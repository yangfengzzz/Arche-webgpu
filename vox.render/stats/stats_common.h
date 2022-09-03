//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <chrono>
#include <string>

#if defined(VK_USE_PLATFORM_XLIB_KHR)
#undef None
#endif

namespace vox {
/**
 * @brief Handles of stats to be optionally enabled in @ref vox::Stats
 */
enum class StatIndex {
    FRAME_TIMES,
    CPU_CYCLES,
    CPU_INSTRUCTIONS,
    CPU_CACHE_MISS_RATIO,
    CPU_BRANCH_MISS_RATIO,
    CPU_L_1_ACCESSES,
    CPU_INSTR_RETIRED,
    CPU_L_2_ACCESSES,
    CPU_L_3_ACCESSES,
    CPU_BUS_READS,
    CPU_BUS_WRITES,
    CPU_MEM_READS,
    CPU_MEM_WRITES,
    CPU_ASE_SPEC,
    CPU_VFP_SPEC,
    CPU_CRYPTO_SPEC,

    GPU_CYCLES,
    GPU_VERTEX_CYCLES,
    GPU_LOAD_STORE_CYCLES,
    GPU_TILES,
    GPU_KILLED_TILES,
    GPU_FRAGMENT_JOBS,
    GPU_FRAGMENT_CYCLES,
    GPU_EXT_READS,
    GPU_EXT_WRITES,
    GPU_EXT_READ_STALLS,
    GPU_EXT_WRITE_STALLS,
    GPU_EXT_READ_BYTES,
    GPU_EXT_WRITE_BYTES,
    GPU_TEX_CYCLES,
};

struct StatIndexHash {
    template <typename T>
    std::size_t operator()(T t) const {
        return static_cast<std::size_t>(t);
    }
};

enum class StatScaling {
    // The stat is not scaled
    NONE,

    // The stat is scaled by delta time, useful for per-second values
    BY_DELTA_TIME,

    // The stat is scaled by another counter, useful for ratios
    BY_COUNTER
};

enum class CounterSamplingMode {
    /// Sample counters only when calling update()
    POLLING,
    /// Sample counters continuously, update circular buffers when calling update()
    CONTINUOUS
};

struct CounterSamplingConfig {
    /// Sampling mode (polling or continuous)
    CounterSamplingMode mode;

    /// Sampling interval in continuous mode
    std::chrono::milliseconds interval{1};

    /// Speed of circular buffer updates in continuous mode;
    /// at speed = 1.0f a new sample is displayed over 1 second.
    float speed{0.5f};
};

// Per-statistic graph data
class StatGraphData {
public:
    /**
     * @brief Constructs data for the graph
     * @param name Name of the Stat
     * @param format Format of the label
     * @param scale_factor Any scaling to apply to the data
     * @param has_fixed_max Whether the data should have a fixed max value
     * @param max_value The maximum value to use
     */
    StatGraphData(std::string name,
                  std::string format,
                  float scale_factor = 1.0f,
                  bool has_fixed_max = false,
                  float max_value = 0.0f);

    StatGraphData() = default;

    std::string name_;
    std::string format_;
    float scale_factor_{};
    bool has_fixed_max_{};
    float max_value_{};
};

}  // namespace vox
