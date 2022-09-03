//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/stats/hwcpipe_stats_provider.h"

#include "vox.render/error.h"

namespace vox {
HWCPipeStatsProvider::HWCPipeStatsProvider(std::set<StatIndex> &requested_stats) {
    // Mapping of stats to their hwcpipe availability
    StatDataMap hwcpipe_stats = {
            {StatIndex::CPU_CYCLES, {hwcpipe::CpuCounter::Cycles}},
            {StatIndex::CPU_INSTRUCTIONS, {hwcpipe::CpuCounter::Instructions}},
            {StatIndex::CPU_CACHE_MISS_RATIO,
             {hwcpipe::CpuCounter::CacheMisses, StatScaling::BY_COUNTER, hwcpipe::CpuCounter::CacheReferences}},
            {StatIndex::CPU_BRANCH_MISS_RATIO,
             {hwcpipe::CpuCounter::BranchMisses, StatScaling::BY_COUNTER, hwcpipe::CpuCounter::BranchInstructions}},
            {StatIndex::CPU_L_1_ACCESSES, {hwcpipe::CpuCounter::L1Accesses}},
            {StatIndex::CPU_INSTR_RETIRED, {hwcpipe::CpuCounter::InstrRetired}},
            {StatIndex::CPU_L_2_ACCESSES, {hwcpipe::CpuCounter::L2Accesses}},
            {StatIndex::CPU_L_3_ACCESSES, {hwcpipe::CpuCounter::L3Accesses}},
            {StatIndex::CPU_BUS_READS, {hwcpipe::CpuCounter::BusReads}},
            {StatIndex::CPU_BUS_WRITES, {hwcpipe::CpuCounter::BusWrites}},
            {StatIndex::CPU_MEM_READS, {hwcpipe::CpuCounter::MemReads}},
            {StatIndex::CPU_MEM_WRITES, {hwcpipe::CpuCounter::MemWrites}},
            {StatIndex::CPU_ASE_SPEC, {hwcpipe::CpuCounter::ASESpec}},
            {StatIndex::CPU_VFP_SPEC, {hwcpipe::CpuCounter::VFPSpec}},
            {StatIndex::CPU_CRYPTO_SPEC, {hwcpipe::CpuCounter::CryptoSpec}},
            {StatIndex::GPU_CYCLES, {hwcpipe::GpuCounter::GpuCycles}},
            {StatIndex::GPU_VERTEX_CYCLES, {hwcpipe::GpuCounter::VertexComputeCycles}},
            {StatIndex::GPU_LOAD_STORE_CYCLES, {hwcpipe::GpuCounter::ShaderLoadStoreCycles}},
            {StatIndex::GPU_TILES, {hwcpipe::GpuCounter::Tiles}},
            {StatIndex::GPU_KILLED_TILES, {hwcpipe::GpuCounter::TransactionEliminations}},
            {StatIndex::GPU_FRAGMENT_CYCLES, {hwcpipe::GpuCounter::FragmentCycles}},
            {StatIndex::GPU_FRAGMENT_JOBS, {hwcpipe::GpuCounter::FragmentJobs}},
            {StatIndex::GPU_EXT_READS, {hwcpipe::GpuCounter::ExternalMemoryReadAccesses}},
            {StatIndex::GPU_EXT_WRITES, {hwcpipe::GpuCounter::ExternalMemoryWriteAccesses}},
            {StatIndex::GPU_EXT_READ_STALLS, {hwcpipe::GpuCounter::ExternalMemoryReadStalls}},
            {StatIndex::GPU_EXT_WRITE_STALLS, {hwcpipe::GpuCounter::ExternalMemoryWriteStalls}},
            {StatIndex::GPU_EXT_READ_BYTES, {hwcpipe::GpuCounter::ExternalMemoryReadBytes}},
            {StatIndex::GPU_EXT_WRITE_BYTES, {hwcpipe::GpuCounter::ExternalMemoryWriteBytes}},
            {StatIndex::GPU_TEX_CYCLES, {hwcpipe::GpuCounter::ShaderTextureCycles}}};

    hwcpipe::CpuCounterSet enabled_cpu_counters{};
    hwcpipe::GpuCounterSet enabled_gpu_counters{};

    for (const auto &stat : requested_stats) {
        auto res = hwcpipe_stats.find(stat);
        if (res != hwcpipe_stats.end()) {
            stat_data_[stat] = hwcpipe_stats[stat];

            switch (res->second.type) {
                case StatType::CPU:
                    enabled_cpu_counters.insert(res->second.cpu_counter);
                    if (res->second.divisor_cpu_counter != hwcpipe::CpuCounter::MaxValue)
                        enabled_cpu_counters.insert(res->second.divisor_cpu_counter);
                    break;
                case StatType::GPU:
                    enabled_gpu_counters.insert(res->second.gpu_counter);
                    if (res->second.divisor_gpu_counter != hwcpipe::GpuCounter::MaxValue)
                        enabled_gpu_counters.insert(res->second.divisor_gpu_counter);
                    break;
            }
        }
    }

    hwcpipe_ = std::make_unique<hwcpipe::HWCPipe>(enabled_cpu_counters, enabled_gpu_counters);

    // Now that we've made a hwcpipe with the counters we'd like, remove any that
    // aren't actually supported
    for (auto iter = stat_data_.begin(); iter != stat_data_.end();) {
        switch (iter->second.type) {
            case StatType::CPU: {
                if (hwcpipe_->cpu_profiler()) {
                    const auto &cpu_supp = hwcpipe_->cpu_profiler()->supported_counters();
                    if (cpu_supp.find(iter->second.cpu_counter) == cpu_supp.end())
                        iter = stat_data_.erase(iter);
                    else
                        ++iter;
                } else {
                    iter = stat_data_.erase(iter);
                }
                break;
            }
            case StatType::GPU: {
                if (hwcpipe_->gpu_profiler()) {
                    const auto &gpu_supp = hwcpipe_->gpu_profiler()->supported_counters();
                    if (gpu_supp.find(iter->second.gpu_counter) == gpu_supp.end())
                        iter = stat_data_.erase(iter);
                    else
                        ++iter;
                } else {
                    iter = stat_data_.erase(iter);
                }
                break;
            }
        }
    }

    // Remove any supported stats from the requested set.
    // Subsequent providers will then only look for things that aren't already supported.
    for (const auto &iter : stat_data_) {
        requested_stats.erase(iter.first);
    }

    hwcpipe_->run();
}

bool HWCPipeStatsProvider::IsAvailable(StatIndex index) const { return stat_data_.find(index) != stat_data_.end(); }

const StatGraphData &HWCPipeStatsProvider::GetGraphData(StatIndex index) const {
    assert(IsAvailable(index) && "HWCPipeStatsProvider::GetGraphData() called with invalid StatIndex");

    static StatGraphData vertex_compute_cycles{"Vertex Compute Cycles", "{:4.1f} M/s", float(1e-6)};

    // HWCPipe reports combined vertex/compute cycles (which is Arm specific)
    // Ensure we report graph with the correct name when asked for vertex cycles
    if (index == StatIndex::GPU_VERTEX_CYCLES) return vertex_compute_cycles;

    return default_graph_map_[index];
}

static double get_cpu_counter_value(const hwcpipe::CpuMeasurements *cpu, hwcpipe::CpuCounter counter) {
    auto hwcpipe_ctr = cpu->find(counter);
    if (hwcpipe_ctr != cpu->end()) return hwcpipe_ctr->second.get<double>();
    return 0.0;
}

static double get_gpu_counter_value(const hwcpipe::GpuMeasurements *gpu, hwcpipe::GpuCounter counter) {
    auto hwcpipe_ctr = gpu->find(counter);
    if (hwcpipe_ctr != gpu->end()) return hwcpipe_ctr->second.get<double>();
    return 0.0;
}

StatsProvider::Counters HWCPipeStatsProvider::Sample(float delta_time) {
    Counters res;
    hwcpipe::Measurements m = hwcpipe_->sample();

    // Map from hwcpipe measurement to our sample result for each counter
    for (auto iter : stat_data_) {
        StatIndex index = iter.first;
        const StatData &data = iter.second;

        double d = 0.0;
        if (data.type == StatType::CPU) {
            d = get_cpu_counter_value(m.cpu, data.cpu_counter);

            if (data.scaling == StatScaling::BY_DELTA_TIME && delta_time != 0.0f) {
                d /= delta_time;
            } else if (data.scaling == StatScaling::BY_COUNTER) {
                double divisor = get_cpu_counter_value(m.cpu, data.divisor_cpu_counter);
                if (divisor != 0.0)
                    d /= divisor;
                else
                    d = 0.0;
            }
        } else if (data.type == StatType::GPU) {
            d = get_gpu_counter_value(m.gpu, data.gpu_counter);

            if (data.scaling == StatScaling::BY_DELTA_TIME && delta_time != 0.0f) {
                d /= delta_time;
            } else if (data.scaling == StatScaling::BY_COUNTER) {
                double divisor = get_gpu_counter_value(m.gpu, data.divisor_gpu_counter);
                if (divisor != 0.0)
                    d /= divisor;
                else
                    d = 0.0;
            }
        }
        res[index].result = d;
    }

    return res;
}

StatsProvider::Counters HWCPipeStatsProvider::ContinuousSample(float delta_time) { return Sample(delta_time); }

}  // namespace vox
