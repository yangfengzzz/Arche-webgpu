//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/stats/vulkan_stats_provider.h"

#include <regex>

#include "vox.render/core/command_buffer.h"
#include "vox.render/core/device.h"
#include "vox.render/error.h"
#include "vox.render/rendering/render_context.h"

namespace vox {
VulkanStatsProvider::VulkanStatsProvider(std::set<StatIndex> &requested_stats,
                                         const CounterSamplingConfig &sampling_config,
                                         RenderContext &render_context)
    : render_context_(render_context) {
    // Check all the Vulkan capabilities we require are present
    if (!IsSupported(sampling_config)) return;

    Device &device = render_context.GetDevice();
    const PhysicalDevice &gpu = device.GetGpu();

    has_timestamps_ = gpu.GetProperties().limits.timestampComputeAndGraphics;
    timestamp_period_ = gpu.GetProperties().limits.timestampPeriod;

    // Interrogate device for supported stats
    uint32_t queue_family_index = device.GetQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);

    // Query number of available counters
    uint32_t count = 0;
    gpu.EnumerateQueueFamilyPerformanceQueryCounters(queue_family_index, &count, nullptr, nullptr);

    if (count == 0) return;  // No counters available

    std::vector<VkPerformanceCounterKHR> counters(count);
    std::vector<VkPerformanceCounterDescriptionKHR> descs(count);

    for (uint32_t i = 0; i < count; i++) {
        counters[i].sType = VK_STRUCTURE_TYPE_PERFORMANCE_COUNTER_KHR;
        counters[i].pNext = nullptr;
        descs[i].sType = VK_STRUCTURE_TYPE_PERFORMANCE_COUNTER_DESCRIPTION_KHR;
        descs[i].pNext = nullptr;
    }

    // Now get the list of counters and their descriptions
    gpu.EnumerateQueueFamilyPerformanceQueryCounters(queue_family_index, &count, counters.data(), descs.data());

    // Every vendor has a different set of performance counters each
    // with different names. Match them to the stats we want, where available.
    if (!FillVendorData()) return;

    bool performance_impact = false;

    // Now build stat_data by matching vendor_data to Vulkan counter data
    for (auto &s : vendor_data_) {
        StatIndex index = s.first;

        if (requested_stats.find(index) == requested_stats.end()) continue;  // We weren't asked for this stat

        VendorStat &init = s.second;
        bool found_ctr = false;
        bool found_div = (init.divisor_name.empty());
        uint32_t ctr_idx, div_idx;

        std::regex name_regex(init.name);
        std::regex div_regex(init.divisor_name);

        for (uint32_t i = 0; !(found_ctr && found_div) && i < descs.size(); i++) {
            if (!found_ctr && std::regex_match(descs[i].name, name_regex)) {
                ctr_idx = i;
                found_ctr = true;
            }
            if (!found_div && std::regex_match(descs[i].name, div_regex)) {
                div_idx = i;
                found_div = true;
            }
        }

        if (found_ctr && found_div) {
            if ((descs[ctr_idx].flags & VK_PERFORMANCE_COUNTER_DESCRIPTION_PERFORMANCE_IMPACTING_KHR) ||
                (!init.divisor_name.empty() &&
                 descs[div_idx].flags != VK_PERFORMANCE_COUNTER_DESCRIPTION_PERFORMANCE_IMPACTING_KHR)) {
                performance_impact = true;
            }

            // Record the counter data
            counter_indices_.emplace_back(ctr_idx);
            if (init.divisor_name.empty()) {
                stat_data_[index] = StatData(ctr_idx, counters[ctr_idx].storage);
            } else {
                counter_indices_.emplace_back(div_idx);
                stat_data_[index] =
                        StatData(ctr_idx, counters[ctr_idx].storage, init.scaling, div_idx, counters[div_idx].storage);
            }
        }
    }

    if (performance_impact) LOGW("The collection of performance counters may impact performance")

    if (counter_indices_.empty()) return;  // No stats available

    // Acquire the profiling lock, without which we can't collect stats
    VkAcquireProfilingLockInfoKHR info{};
    info.sType = VK_STRUCTURE_TYPE_ACQUIRE_PROFILING_LOCK_INFO_KHR;
    info.timeout = 2000000000;  // 2 seconds (in ns)

    if (vkAcquireProfilingLockKHR(device.GetHandle(), &info) != VK_SUCCESS) {
        stat_data_.clear();
        counter_indices_.clear();
        LOGW("Profiling lock acquisition timed-out")
        return;
    }

    // Now we know the counters and that we can collect them, make a query pool for the results.
    if (!CreateQueryPools(queue_family_index)) {
        stat_data_.clear();
        counter_indices_.clear();
        return;
    }

    // These stats are fully supported by this provider and in a single pass, so remove
    // from the requested set.
    // Subsequent providers will then only look for things that aren't already supported.
    for (const auto &s : stat_data_) requested_stats.erase(s.first);
}

VulkanStatsProvider::~VulkanStatsProvider() {
    if (!stat_data_.empty()) {
        // Release profiling lock
        vkReleaseProfilingLockKHR(render_context_.GetDevice().GetHandle());
    }
}

bool VulkanStatsProvider::FillVendorData() {
    const auto &pd_props = render_context_.GetDevice().GetGpu().GetProperties();
    if (pd_props.vendorID == 0x14E4)  // Broadcom devices
    {
        LOGI("Using Vulkan performance counters from Broadcom device")

        // NOTE: The names here are actually regular-expressions.
        // Counter names can change between hardware variants for the same vendor,
        // so regular expression names mean that multiple h/w variants can be easily supported.
        // clang-format off
        vendor_data_ = {
            {StatIndex::GPU_CYCLES, {VendorStat("cycle_count")}},
            {StatIndex::GPU_VERTEX_CYCLES, {VendorStat("gpu_vertex_cycles")}},
            {StatIndex::GPU_FRAGMENT_CYCLES, {VendorStat("gpu_fragment_cycles")}},
            {StatIndex::GPU_FRAGMENT_JOBS, {VendorStat("render_jobs_completed")}},
            {StatIndex::GPU_EXT_READS, {VendorStat("gpu_mem_reads")}},
            {StatIndex::GPU_EXT_WRITES, {VendorStat("gpu_mem_writes")}},
            {StatIndex::GPU_EXT_READ_BYTES, {VendorStat("gpu_bytes_read")}},
            {StatIndex::GPU_EXT_WRITE_BYTES, {VendorStat("gpu_bytes_written")}},
        };
        // clang-format on

        // Override vendor-specific graph data
        vendor_data_.at(StatIndex::GPU_VERTEX_CYCLES)
                .SetVendorGraphData({"Vertex/Coord/User Cycles", "{:4.1f} M/s", float(1e-6)});
        vendor_data_.at(StatIndex::GPU_FRAGMENT_JOBS).SetVendorGraphData({"Render Jobs", "{:4.0f}/s"});

        return true;
    }
#if 0
    else if (pd_props.vendorID == xxxx) // Other vendor's devices
    {
        // Fill vendor_data for other vendor
        return true;
    }
#endif
    {
        // Unsupported vendor
        return false;
    }
}

bool VulkanStatsProvider::CreateQueryPools(uint32_t queue_family_index) {
    Device &device = render_context_.GetDevice();
    const PhysicalDevice &gpu = device.GetGpu();
    auto num_framebuffers = uint32_t(render_context_.GetRenderFrames().size());

    // Now we know the available counters, we can build a query pool that will collect them.
    // We will check that the counters can be collected in a single pass. Multi-pass would
    // be a big performance hit so for these samples, we don't want to use it.
    VkQueryPoolPerformanceCreateInfoKHR perf_create_info{};
    perf_create_info.sType = VK_STRUCTURE_TYPE_QUERY_POOL_PERFORMANCE_CREATE_INFO_KHR;
    perf_create_info.queueFamilyIndex = queue_family_index;
    perf_create_info.counterIndexCount = uint32_t(counter_indices_.size());
    perf_create_info.pCounterIndices = counter_indices_.data();

    uint32_t passes_needed = gpu.GetQueueFamilyPerformanceQueryPasses(&perf_create_info);
    if (passes_needed != 1) {
        // Needs more than one pass, remove all our supported stats
        LOGW("Requested Vulkan stats require multiple passes, we won't collect them")
        return false;
    }

    // We will need a query pool to report the stats back to us
    VkQueryPoolCreateInfo pool_create_info{};
    pool_create_info.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
    pool_create_info.pNext = &perf_create_info;
    pool_create_info.queryType = VK_QUERY_TYPE_PERFORMANCE_QUERY_KHR;
    pool_create_info.queryCount = num_framebuffers;

    query_pool_ = std::make_unique<QueryPool>(device, pool_create_info);

    if (!query_pool_) {
        LOGW("Failed to create performance query pool")
        return false;
    }

    // Reset the query pool before first use. We cannot do these in the command buffer
    // as that is invalid usage for performance queries due to the potential for multiple
    // passes being required.
    query_pool_->HostReset(0, num_framebuffers);

    if (has_timestamps_) {
        // If we support timestamp queries we will use those to more accurately measure
        // the time spent executing a command buffer than just a frame-to-frame timer
        // in software.
        VkQueryPoolCreateInfo timestamp_pool_create_info{};
        timestamp_pool_create_info.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
        timestamp_pool_create_info.queryType = VK_QUERY_TYPE_TIMESTAMP;
        timestamp_pool_create_info.queryCount = num_framebuffers * 2;  // 2 timestamps per frame (start & end)

        timestamp_pool_ = std::make_unique<QueryPool>(device, timestamp_pool_create_info);
    }

    return true;
}

bool VulkanStatsProvider::IsSupported(const CounterSamplingConfig &sampling_config) const {
    // Continuous sampling mode cannot be supported by VK_KHR_performance_query
    if (sampling_config.mode == CounterSamplingMode::CONTINUOUS) return false;

    Device &device = render_context_.GetDevice();

    // The VK_KHR_performance_query must be available and enabled
    if (!(device.IsEnabled("VK_KHR_performance_query") && device.IsEnabled("VK_EXT_host_query_reset"))) return false;

    // Check the performance query features flag.
    // Note: VK_KHR_get_physical_device_properties2 is a pre-requisite of VK_KHR_performance_query
    // so must be present.
    VkPhysicalDevicePerformanceQueryFeaturesKHR perf_query_features{};
    perf_query_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_FEATURES_KHR;

    VkPhysicalDeviceFeatures2KHR device_features{};
    device_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR;
    device_features.pNext = &perf_query_features;

    vkGetPhysicalDeviceFeatures2(device.GetGpu().GetHandle(), &device_features);
    if (!perf_query_features.performanceCounterQueryPools) return false;

    return true;
}

bool VulkanStatsProvider::IsAvailable(StatIndex index) const { return stat_data_.find(index) != stat_data_.end(); }

const StatGraphData &VulkanStatsProvider::GetGraphData(StatIndex index) const {
    assert(IsAvailable(index) && "VulkanStatsProvider::GetGraphData() called with invalid StatIndex");

    const auto &data = vendor_data_.find(index)->second;
    if (data.has_vendor_graph_data) return data.graph_data;

    return default_graph_map_[index];
}

void VulkanStatsProvider::BeginSampling(CommandBuffer &cb) {
    uint32_t active_frame_idx = render_context_.GetActiveFrameIndex();
    if (timestamp_pool_) {
        // We use TimestampQueries when available to provide a more accurate delta_time.
        // These counters are from a single command buffer execution, but the passed
        // delta time is a frame-to-frame s/w measure. A timestamp query in the cmd
        // buffer gives the actual elapsed time when the counters were measured.
        cb.ResetQueryPool(*timestamp_pool_, active_frame_idx * 2, 1);
        cb.WriteTimestamp(VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, *timestamp_pool_, active_frame_idx * 2);
    }

    if (query_pool_) cb.BeginQuery(*query_pool_, active_frame_idx, VkQueryControlFlags(0));
}

void VulkanStatsProvider::EndSampling(CommandBuffer &cb) {
    uint32_t active_frame_idx = render_context_.GetActiveFrameIndex();

    if (query_pool_) {
        // Perform a barrier to ensure all previous commands complete before ending the query
        // This does not block later commands from executing as we use BOTTOM_OF_PIPE in the
        // dst stage mask
        vkCmdPipelineBarrier(cb.GetHandle(), VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                             0, 0, nullptr, 0, nullptr, 0, nullptr);
        cb.EndQuery(*query_pool_, active_frame_idx);

        ++queries_ready_;
    }

    if (timestamp_pool_) {
        cb.ResetQueryPool(*timestamp_pool_, active_frame_idx * 2 + 1, 1);
        cb.WriteTimestamp(VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, *timestamp_pool_, active_frame_idx * 2 + 1);
    }
}

static double GetCounterValue(const VkPerformanceCounterResultKHR &result, VkPerformanceCounterStorageKHR storage) {
    switch (storage) {
        case VK_PERFORMANCE_COUNTER_STORAGE_INT32_KHR:
            return double(result.int32);
        case VK_PERFORMANCE_COUNTER_STORAGE_INT64_KHR:
            return double(result.int64);
        case VK_PERFORMANCE_COUNTER_STORAGE_UINT32_KHR:
            return double(result.uint32);
        case VK_PERFORMANCE_COUNTER_STORAGE_UINT64_KHR:
            return double(result.uint64);
        case VK_PERFORMANCE_COUNTER_STORAGE_FLOAT32_KHR:
            return double(result.float32);
        case VK_PERFORMANCE_COUNTER_STORAGE_FLOAT64_KHR:
            return double(result.float64);
        default:
            assert(0);
            throw std::runtime_error("");
    }
}

float VulkanStatsProvider::GetBestDeltaTime(float sw_delta_time) const {
    if (!timestamp_pool_) return sw_delta_time;

    float delta_time = sw_delta_time;

    // Query the timestamps to get an accurate delta time
    std::array<uint64_t, 2> timestamps{};

    uint32_t active_frame_idx = render_context_.GetActiveFrameIndex();

    VkResult r = timestamp_pool_->GetResults(active_frame_idx * 2, 2, timestamps.size() * sizeof(uint64_t),
                                             timestamps.data(), sizeof(uint64_t),
                                             VK_QUERY_RESULT_WAIT_BIT | VK_QUERY_RESULT_64_BIT);
    if (r == VK_SUCCESS) {
        float elapsed_ns = timestamp_period_ * float(timestamps[1] - timestamps[0]);
        delta_time = elapsed_ns * 0.000000001f;
    }

    return delta_time;
}

StatsProvider::Counters VulkanStatsProvider::Sample(float delta_time) {
    Counters out;
    if (!query_pool_ || queries_ready_ == 0) return out;

    uint32_t active_frame_idx = render_context_.GetActiveFrameIndex();

    VkDeviceSize stride = sizeof(VkPerformanceCounterResultKHR) * counter_indices_.size();

    std::vector<VkPerformanceCounterResultKHR> results(counter_indices_.size());

    VkResult r = query_pool_->GetResults(active_frame_idx, 1, results.size() * sizeof(VkPerformanceCounterResultKHR),
                                         results.data(), stride, VK_QUERY_RESULT_WAIT_BIT);
    if (r != VK_SUCCESS) return out;

    // Use timestamps to get a more accurate delta if available
    delta_time = GetBestDeltaTime(delta_time);

    // Parse the results - they are in the order we gave in counter_indices
    for (const auto &s : stat_data_) {
        StatIndex si = s.first;

        bool need_divisor = (stat_data_[si].scaling == StatScaling::BY_COUNTER);
        double divisor_value = 1.0;
        double value = 0.0;
        bool found_ctr = false, found_div = !need_divisor;

        for (uint32_t i = 0; !(found_ctr && found_div) && i < counter_indices_.size(); i++) {
            if (s.second.counter_index == counter_indices_[i]) {
                value = GetCounterValue(results[i], stat_data_[si].storage);
                found_ctr = true;
            }
            if (need_divisor && s.second.divisor_counter_index == counter_indices_[i]) {
                divisor_value = GetCounterValue(results[i], stat_data_[si].divisor_storage);
                found_div = true;
            }
        }

        if (found_ctr && found_div) {
            if (stat_data_[si].scaling == StatScaling::BY_DELTA_TIME && delta_time != 0.0)
                value /= delta_time;
            else if (stat_data_[si].scaling == StatScaling::BY_COUNTER && divisor_value != 0.0)
                value /= divisor_value;
            out[si].result = value;
        }
    }

    // Now reset the query we just fetched the results from
    query_pool_->HostReset(active_frame_idx, 1);

    --queries_ready_;

    return out;
}

}  // namespace vox
