//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/error.h"
#include "vox.render/vk_common.h"

VKBP_DISABLE_WARNINGS()

#include <hwcpipe.h>

VKBP_ENABLE_WARNINGS()

#include "vox.render/stats/stats_provider.h"

namespace vox {
class HWCPipeStatsProvider : public StatsProvider {
private:
    enum class StatType { CPU, GPU };

    struct StatData {
        StatType type;
        StatScaling scaling;
        hwcpipe::CpuCounter cpu_counter;
        hwcpipe::CpuCounter divisor_cpu_counter;
        hwcpipe::GpuCounter gpu_counter;
        hwcpipe::GpuCounter divisor_gpu_counter;

        StatData() = default;

        /**
         * @brief Constructor for CPU counters
         * @param c The CPU counter to be gathered
         * @param stat_scaling The scaling to be applied to the stat
         * @param divisor The CPU counter to be used as divisor if scaling is ByCounter
         */
        StatData(hwcpipe::CpuCounter c,
                 StatScaling stat_scaling = StatScaling::BY_DELTA_TIME,
                 hwcpipe::CpuCounter divisor = hwcpipe::CpuCounter::MaxValue)
            : type(StatType::CPU), scaling(stat_scaling), cpu_counter(c), divisor_cpu_counter(divisor) {}

        /**
         * @brief Constructor for GPU counters
         * @param c The GPU counter to be gathered
         * @param stat_scaling The scaling to be applied to the stat
         * @param divisor The GPU counter to be used as divisor if scaling is ByCounter
         */
        StatData(hwcpipe::GpuCounter c,
                 StatScaling stat_scaling = StatScaling::BY_DELTA_TIME,
                 hwcpipe::GpuCounter divisor = hwcpipe::GpuCounter::MaxValue)
            : type(StatType::GPU), scaling(stat_scaling), gpu_counter(c), divisor_gpu_counter(divisor) {}
    };

    using StatDataMap = std::unordered_map<StatIndex, StatData, StatIndexHash>;

public:
    /**
     * @brief Constructs a HWCPipeStateProvider
     * @param requested_stats Set of stats to be collected. Supported stats will be removed from the set.
     */
    explicit HWCPipeStatsProvider(std::set<StatIndex> &requested_stats);

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
     * @brief Retrieve a new sample set from continuous sampling
     * @param delta_time Time since last sample
     */
    Counters ContinuousSample(float delta_time) override;

private:
    // The hwcpipe instance
    std::unique_ptr<hwcpipe::HWCPipe> hwcpipe_{};

    // Only stats which are available and were requested end up in stat_data
    StatDataMap stat_data_;

    // Counter sampling configuration
    CounterSamplingConfig sampling_config_;
};

}  // namespace vox
