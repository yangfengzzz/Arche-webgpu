//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cstdint>
#include <ctime>
#include <future>
#include <map>
#include <set>
#include <vector>

#include "vox.base/timer.h"
#include "vox.render/error.h"
#include "vox.render/stats/stats_common.h"
#include "vox.render/stats/stats_provider.h"

namespace vox {
class Device;

class CommandBuffer;

class RenderContext;

/*
 * @brief Helper class for querying statistics about the CPU and the GPU
 */
class Stats {
public:
    /**
     * @brief Constructs a Stats object
     * @param render_context The RenderContext for this sample
     * @param buffer_size Size of the circular buffers
     */
    explicit Stats(RenderContext &render_context, size_t buffer_size = 16);

    /**
     * @brief Destroys the Stats object
     */
    ~Stats();

    /**
     * @brief Request specific set of stats to be collected
     * @param requested_stats Set of stats to be collected if available
     * @param sampling_config Sampling mode configuration (polling or continuous)
     */
    void RequestStats(const std::set<StatIndex> &requested_stats,
                      CounterSamplingConfig sampling_config = {CounterSamplingMode::POLLING});

    /**
     * @brief Resizes the stats buffers according to the width of the screen
     * @param width The width of the screen
     */
    void Resize(size_t width);

    /**
     * @brief Checks if an enabled stat is available in the current platform
     * @param index The stat index
     * @return True if the stat is available, false otherwise
     */
    [[nodiscard]] bool IsAvailable(StatIndex index) const;

    /**
     * @brief Returns data relevant for graphing a specific statistic
     * @param index The stat index of the data requested
     * @return The data of the specified stat
     */
    [[nodiscard]] const StatGraphData &GetGraphData(StatIndex index) const;

    /**
     * @brief Returns the collected data for a specific statistic
     * @param index The stat index of the data requested
     * @return The data of the specified stat
     */
    [[nodiscard]] const std::vector<float> &GetData(StatIndex index) const { return counters_.at(index); };

    /**
     * @return The requested stats
     */
    [[nodiscard]] const std::set<StatIndex> &GetRequestedStats() const { return requested_stats_; }

    /**
     * @brief Update statistics, must be called after every frame
     * @param delta_time Time since last update
     */
    void Update(float delta_time);

    /**
     * @brief A command buffer that we want to collect stats about has just begun
     *
     * Some stats providers (like the Vulkan extension one) can only collect stats
     * about the execution of a specific command buffer. In those cases we need to
     * know when a command buffer has begun and when it's about to end so that we
     * can inject some extra commands into the command buffer to control the stats
     * collection. This method tells the stats provider that a command buffer has
     * begun so that can happen. The command buffer must be in a recording state
     * when this method is called.
     * @param cb The command buffer
     */
    void BeginSampling(CommandBuffer &cb);

    /**
     * @brief A command buffer that we want to collect stats about is about to be ended
     *
     * Some stats providers (like the Vulkan extension one) can only collect stats
     * about the execution of a specific command buffer. In those cases we need to
     * know when a command buffer has begun and when it's about to end so that we
     * can inject some extra commands into the command buffer to control the stats
     * collection. This method tells the stats provider that a command buffer is
     * about to be ended so that can happen. The command buffer must be in a recording
     * state when this method is called.
     * @param cb The command buffer
     */
    void EndSampling(CommandBuffer &cb);

private:
    /// The render context
    RenderContext &render_context_;

    /// Stats that were requested - they may not all be available
    std::set<StatIndex> requested_stats_;

    /// Provider that tracks frame times
    StatsProvider *frame_time_provider_{nullptr};

    /// A list of stats providers to use in priority order
    std::vector<std::unique_ptr<StatsProvider>> providers_;

    /// Counter sampling configuration
    CounterSamplingConfig sampling_config_;

    /// Size of the circular buffers
    size_t buffer_size_;

    /// Timer used in the main thread to compute delta time
    utility::Timer main_timer_;

    /// Timer used by the worker thread to throttle counter sampling
    utility::Timer worker_timer_;

    /// Alpha smoothing for running average
    float alpha_smoothing_{0.2f};

    /// Circular buffers for counter data
    std::map<StatIndex, std::vector<float>> counters_{};

    /// Worker thread for continuous sampling
    std::thread worker_thread_;

    /// Promise to stop the worker thread
    std::unique_ptr<std::promise<void>> stop_worker_;

    /// A mutex for accessing measurements during continuous sampling
    std::mutex continuous_sampling_mutex_;

    /// The samples read during continuous sampling
    std::vector<StatsProvider::Counters> continuous_samples_;

    /// A flag specifying if the worker thread should add entries to continuous_samples
    bool should_add_to_continuous_samples_{false};

    /// The samples waiting to be displayed
    std::vector<StatsProvider::Counters> pending_samples_;

    /// A value which helps keep a steady pace of continuous samples output.
    float fractional_pending_samples_{0.0f};

    /// The worker thread function for continuous sampling;
    /// it adds a new entry to continuous_samples at every interval
    void ContinuousSamplingWorker(std::future<void> should_terminate);

    /// Updates circular buffers for CPU and GPU counters
    void PushSample(const StatsProvider::Counters &sample);
};

}  // namespace vox
