//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>

namespace vox::utility {

/**
 * @brief Encapsulates basic usage of chrono, providing a means to calculate float
 *        durations between time points via function calls.
 */
class Timer {
public:
    using Seconds = std::ratio<1>;
    using Milliseconds = std::ratio<1, 1000>;
    using Microseconds = std::ratio<1, 1000000>;
    using Nanoseconds = std::ratio<1, 1000000000>;

    // Configure
    using Clock = std::chrono::steady_clock;
    using DefaultResolution = Seconds;

    Timer();

    virtual ~Timer() = default;

    /**
     * @brief Starts the timer, elapsed() now returns the duration since start()
     */
    void Start();

    /**
     * @brief Laps the timer, elapsed() now returns the duration since the last lap()
     */
    void Lap();

    /**
     * @brief Stops the timer, elapsed() now returns 0
     * @return The total execution time between `start()` and `stop()`
     */
    template <typename T = DefaultResolution>
    double Stop() {
        if (!running_) {
            return 0;
        }

        running_ = false;
        lapping_ = false;
        auto duration = std::chrono::duration<double, T>(Clock::now() - start_time_);
        start_time_ = Clock::now();
        lap_time_ = Clock::now();

        return duration.count();
    }

    /**
     * @brief Calculates the time difference between now and when the timer was started
     *        if lap() was called, then between now and when the timer was last lapped
     * @return The duration between the two time points (default in seconds)
     */
    template <typename T = DefaultResolution>
    double Elapsed() {
        if (!running_) {
            return 0;
        }

        Clock::time_point start = start_time_;

        if (lapping_) {
            start = lap_time_;
        }

        return std::chrono::duration<double, T>(Clock::now() - start).count();
    }

    /**
     * @brief Calculates the time difference between now and the last time this function was called
     * @return The duration between the two time points (default in seconds)
     */
    template <typename T = DefaultResolution>
    double Tick() {
        auto now = Clock::now();
        auto duration = std::chrono::duration<double, T>(now - previous_tick_);
        previous_tick_ = now;
        return duration.count();
    }

    /**
     * @brief Check if the timer is running
     */
    [[nodiscard]] bool IsRunning() const;

private:
    bool running_{false};

    bool lapping_{false};

    Clock::time_point start_time_;

    Clock::time_point lap_time_;

    Clock::time_point previous_tick_;
};

// MARK: - ScopeTimer
class ScopeTimer : public Timer {
public:
    explicit ScopeTimer(std::string scope_timer_info = "");
    ~ScopeTimer();

private:
    std::string scope_timer_info_;
};

// MARK: - FPSTimer
class FPSTimer : public Timer {
public:
    explicit FPSTimer(std::string fps_timer_info = "",
                      int expectation = -1,
                      double time_to_print = 3000.0,
                      int events_to_print = 100);

    /// Function to signal an event
    /// It automatically prints FPS information when duration is more than
    /// time_to_print_, or event has been signaled events_to_print_ times.
    void Signal();

private:
    std::string fps_timer_info_;
    int expectation_;
    double time_to_print_;
    int events_to_print_;
    int event_fragment_count_;
    int event_total_count_;
};

}  // namespace vox::utility
