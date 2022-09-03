//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/stats/stats.h"

#include <utility>

#include "vox.render/core/device.h"
#include "vox.render/error.h"
#include "vox.render/stats/frame_time_stats_provider.h"
#include "vox.render/stats/hwcpipe_stats_provider.h"
#include "vox.render/stats/vulkan_stats_provider.h"

namespace vox {
Stats::Stats(RenderContext &render_context, size_t buffer_size)
    : render_context_(render_context), buffer_size_(buffer_size) {
    assert(buffer_size >= 2 && "Buffers size should be greater than 2");
}

Stats::~Stats() {
    if (stop_worker_) {
        stop_worker_->set_value();
    }

    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
}

void Stats::RequestStats(const std::set<StatIndex> &requested_stats, CounterSamplingConfig sampling_config) {
    if (!providers_.empty()) {
        throw std::runtime_error("Stats must only be requested once");
    }

    requested_stats_ = requested_stats;
    sampling_config_ = sampling_config;

    // Copy the requested stats, so they can be changed by the providers below
    std::set<StatIndex> stats = requested_stats_;

    // Initialize our list of providers (in priority order)
    // All supported stats will be removed from the given 'stats' set by the provider's constructor
    // so subsequent providers only see requests for stats that aren't already supported.
    providers_.emplace_back(std::make_unique<FrameTimeStatsProvider>(stats));
    providers_.emplace_back(std::make_unique<HWCPipeStatsProvider>(stats));
    providers_.emplace_back(std::make_unique<VulkanStatsProvider>(stats, sampling_config_, render_context_));

    // In continuous sampling mode we still need to update the frame times as if we are polling
    // Store the frame time provider here, so we can easily access it later.
    frame_time_provider_ = providers_[0].get();

    for (const auto &stat : requested_stats_) {
        counters_[stat] = std::vector<float>(buffer_size_, 0);
    }

    if (sampling_config_.mode == CounterSamplingMode::CONTINUOUS) {
        // Start a thread for continuous sample capture
        stop_worker_ = std::make_unique<std::promise<void>>();

        worker_thread_ = std::thread([this] { ContinuousSamplingWorker(stop_worker_->get_future()); });

        // Reduce smoothing for continuous sampling
        alpha_smoothing_ = 0.6f;
    }

    for (const auto &stat_index : requested_stats_) {
        if (!IsAvailable(stat_index)) {
            LOGW(vox::StatsProvider::DefaultGraphData(stat_index).name_ + " : not available")
        }
    }
}

void Stats::Resize(const size_t width) {
    // The circular buffer size will be 1/16th of the width of the screen
    // which means every sixteen pixels represent one graph value
    buffer_size_ = width >> 4;

    for (auto &counter : counters_) {
        counter.second.resize(buffer_size_);
        counter.second.shrink_to_fit();
    }
}

bool Stats::IsAvailable(StatIndex index) const {
    for (const auto &p : providers_)
        if (p->IsAvailable(index)) return true;
    return false;
}

static void AddSmoothedValue(std::vector<float> &values, float value, float alpha) {
    assert(values.size() >= 2 && "Buffers size should be greater than 2");

    if (values.size() == values.capacity()) {
        // Shift values to the left to make space at the end and update counters
        std::rotate(values.begin(), values.begin() + 1, values.end());
    }

    // Use an exponential moving average to smooth values
    values.back() = value * alpha + *(values.end() - 2) * (1.0f - alpha);
}

void Stats::Update(float delta_time) {
    switch (sampling_config_.mode) {
        case CounterSamplingMode::POLLING: {
            StatsProvider::Counters sample;

            for (auto &p : providers_) {
                auto s = p->Sample(delta_time);
                sample.insert(s.begin(), s.end());
            }
            PushSample(sample);
            break;
        }
        case CounterSamplingMode::CONTINUOUS: {
            // Check that we have no pending samples to be shown
            if (pending_samples_.empty()) {
                std::unique_lock<std::mutex> lock(continuous_sampling_mutex_);
                if (!should_add_to_continuous_samples_) {
                    // If we have no pending samples, we let the worker thread
                    // capture samples for the next frame
                    should_add_to_continuous_samples_ = true;
                } else {
                    // The worker thread has captured a frame, so we stop it
                    // and read the samples
                    should_add_to_continuous_samples_ = false;
                    pending_samples_.clear();
                    std::swap(pending_samples_, continuous_samples_);
                }
            }

            if (pending_samples_.empty()) return;

            // Ensure the number of pending samples is capped at a reasonable value
            if (pending_samples_.size() > 100) {
                // Prefer later samples over new samples.
                std::move(pending_samples_.end() - 100, pending_samples_.end(), pending_samples_.begin());
                pending_samples_.erase(pending_samples_.begin() + 100, pending_samples_.end());

                // If we get to this point, we're not reading samples fast enough, nudge a little ahead.
                fractional_pending_samples_ += 1.0f;
            }

            // Compute the number of samples to show this frame
            float floating_sample_count =
                    sampling_config_.speed * delta_time * float(buffer_size_) + fractional_pending_samples_;

            // Keep track of the fractional value to avoid speeding up or slowing down too much due to rounding errors.
            // Generally we push very few samples per frame, so this matters.
            fractional_pending_samples_ = floating_sample_count - std::floor(floating_sample_count);

            auto sample_count = static_cast<size_t>(floating_sample_count);

            // Clamp the number of samples
            sample_count = std::max<size_t>(1, std::min<size_t>(sample_count, pending_samples_.size()));

            // Get the frame time stats (not a continuous stat)
            StatsProvider::Counters frame_time_sample = frame_time_provider_->Sample(delta_time);

            // Push the samples to circular buffers
            std::for_each(pending_samples_.begin(), pending_samples_.begin() + sample_count,
                          [this, frame_time_sample](auto &s) {
                              // Write the correct frame time into the continuous stats
                              s.insert(frame_time_sample.begin(), frame_time_sample.end());
                              // Then push the sample to the counters list
                              this->PushSample(s);
                          });
            pending_samples_.erase(pending_samples_.begin(), pending_samples_.begin() + sample_count);

            break;
        }
    }
}

void Stats::ContinuousSamplingWorker(std::future<void> should_terminate) {
    worker_timer_.Tick();

    for (auto &p : providers_) p->ContinuousSample(0.0f);

    while (should_terminate.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
        auto delta_time = static_cast<float>(worker_timer_.Tick());
        auto interval = std::chrono::duration_cast<std::chrono::duration<float>>(sampling_config_.interval).count();

        // Ensure we wait for the interval specified in config
        if (delta_time < interval) {
            std::this_thread::sleep_for(std::chrono::duration<float>(interval - delta_time));
            delta_time += static_cast<float>(worker_timer_.Tick());
        }

        // Sample counters
        StatsProvider::Counters sample;
        for (auto &p : providers_) {
            StatsProvider::Counters s = p->ContinuousSample(delta_time);
            sample.insert(s.begin(), s.end());
        }

        // Add the new sample to the vector of continuous samples
        {
            std::unique_lock<std::mutex> lock(continuous_sampling_mutex_);
            if (should_add_to_continuous_samples_) {
                continuous_samples_.push_back(sample);
            }
        }
    }
}

void Stats::PushSample(const StatsProvider::Counters &sample) {
    for (auto &c : counters_) {
        StatIndex idx = c.first;
        std::vector<float> &values = c.second;

        // Find the counter matching this StatIndex in the Sample
        const auto &smp = sample.find(idx);
        if (smp == sample.end()) continue;

        auto measurement = static_cast<float>(smp->second.result);

        AddSmoothedValue(values, measurement, alpha_smoothing_);
    }
}

void Stats::BeginSampling(CommandBuffer &cb) {
    // Inform the providers
    for (auto &p : providers_) p->BeginSampling(cb);
}

void Stats::EndSampling(CommandBuffer &cb) {
    // Inform the providers
    for (auto &p : providers_) p->EndSampling(cb);
}

const StatGraphData &Stats::GetGraphData(StatIndex index) const {
    for (auto &p : providers_) {
        if (p->IsAvailable(index)) return p->GetGraphData(index);
    }
    return StatsProvider::DefaultGraphData(index);
}

StatGraphData::StatGraphData(
        std::string name, std::string graph_label_format, float scale_factor, bool has_fixed_max, float max_value)
    : name_(std::move(name)),
      format_{std::move(graph_label_format)},
      scale_factor_{scale_factor},
      has_fixed_max_{has_fixed_max},
      max_value_{max_value} {}

}  // namespace vox
