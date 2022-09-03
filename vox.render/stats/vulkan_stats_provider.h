//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <utility>

#include "vox.render/core/query_pool.h"
#include "vox.render/stats/stats_provider.h"

namespace vox {
class RenderContext;

class VulkanStatsProvider : public StatsProvider {
private:
    struct StatData {
        StatScaling scaling;
        uint32_t counter_index{};
        uint32_t divisor_counter_index{};
        VkPerformanceCounterStorageKHR storage;
        VkPerformanceCounterStorageKHR divisor_storage;
        StatGraphData graph_data;

        StatData() = default;

        StatData(uint32_t counter_index,
                 VkPerformanceCounterStorageKHR storage,
                 StatScaling stat_scaling = StatScaling::BY_DELTA_TIME,
                 uint32_t divisor_index = std::numeric_limits<uint32_t>::max(),
                 VkPerformanceCounterStorageKHR divisor_storage = VK_PERFORMANCE_COUNTER_STORAGE_FLOAT64_KHR)
            : scaling(stat_scaling),
              counter_index(counter_index),
              divisor_counter_index(divisor_index),
              storage(storage),
              divisor_storage(divisor_storage) {}
    };

    struct VendorStat {
        explicit VendorStat(std::string name, const std::string &divisor_name = "")
            : name(std::move(name)), divisor_name(divisor_name) {
            if (!divisor_name.empty()) scaling = StatScaling::BY_COUNTER;
        }

        void SetVendorGraphData(const StatGraphData &data) {
            has_vendor_graph_data = true;
            graph_data = data;
        }

        std::string name;
        StatScaling scaling = StatScaling::BY_DELTA_TIME;
        std::string divisor_name;
        bool has_vendor_graph_data = false;
        StatGraphData graph_data;
    };

    using StatDataMap = std::unordered_map<StatIndex, StatData, StatIndexHash>;
    using VendorStatMap = std::unordered_map<StatIndex, VendorStat, StatIndexHash>;

public:
    /**
     * @brief Constructs a VulkanStatsProvider
     * @param requested_stats Set of stats to be collected. Supported stats will be removed from the set.
     * @param sampling_config Sampling mode configuration (polling or continuous)
     * @param render_context The render context
     */
    VulkanStatsProvider(std::set<StatIndex> &requested_stats,
                        const CounterSamplingConfig &sampling_config,
                        RenderContext &render_context);

    /**
     * @brief Destructs a VulkanStatsProvider
     */
    ~VulkanStatsProvider() override;

    /**
     * @brief Checks if this provider can supply the given enabled stat
     * @param index The stat index
     * @return True if the stat is available, false otherwise
     */
    [[nodiscard]] bool IsAvailable(StatIndex index) const override;

    /**
     * @brief Retrieve graphing data for the given enabled stat
     * @param index The stat index
     */
    [[nodiscard]] const StatGraphData &GetGraphData(StatIndex index) const override;

    /**
     * @brief Retrieve a new sample set from polled sampling
     * @param delta_time Time since last sample
     */
    Counters Sample(float delta_time) override;

    /**
     * @brief A command buffer that we want stats about has just begun
     * @param cb The command buffer
     */
    void BeginSampling(CommandBuffer &cb) override;

    /**
     * @brief A command buffer that we want stats about is about to be ended
     * @param cb The command buffer
     */
    void EndSampling(CommandBuffer &cb) override;

private:
    [[nodiscard]] bool IsSupported(const CounterSamplingConfig &sampling_config) const;

    bool FillVendorData();

    bool CreateQueryPools(uint32_t queue_family_index);

    [[nodiscard]] float GetBestDeltaTime(float sw_delta_time) const;

private:
    // The render context
    RenderContext &render_context_;

    // The query pool for the performance queries
    std::unique_ptr<QueryPool> query_pool_;

    // Do we support timestamp queries
    bool has_timestamps_{false};

    // The timestamp period
    float timestamp_period_{1.0f};

    // Query pool for timestamps
    std::unique_ptr<QueryPool> timestamp_pool_;

    // Map of vendor specific stat data
    VendorStatMap vendor_data_;

    // Only stats which are available and were requested end up in stat_data
    StatDataMap stat_data_;

    // An ordered list of the Vulkan counter ids
    std::vector<uint32_t> counter_indices_;

    // How many queries have been ended?
    uint32_t queries_ready_ = 0;
};

}  // namespace vox
