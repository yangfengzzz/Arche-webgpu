//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <chrono>

namespace vox {
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
    void start();
    
    /**
     * @brief Laps the timer, elapsed() now returns the duration since the last lap()
     */
    void lap();
    
    /**
     * @brief Stops the timer, elapsed() now returns 0
     * @return The total execution time between `start()` and `stop()`
     */
    template<typename T = DefaultResolution>
    double stop() {
        if (!_running) {
            return 0;
        }
        
        _running = false;
        _lapping = false;
        auto duration = std::chrono::duration<double, T>(Clock::now() - _startTime);
        _startTime = Clock::now();
        _lapTime = Clock::now();
        
        return duration.count();
    }
    
    /**
     * @brief Calculates the time difference between now and when the timer was started
     *        if lap() was called, then between now and when the timer was last lapped
     * @return The duration between the two time points (default in seconds)
     */
    template<typename T = DefaultResolution>
    double elapsed() {
        if (!_running) {
            return 0;
        }
        
        Clock::time_point start = _startTime;
        
        if (_lapping) {
            start = _lapTime;
        }
        
        return std::chrono::duration<double, T>(Clock::now() - start).count();
    }
    
    /**
     * @brief Calculates the time difference between now and the last time this function was called
     * @return The duration between the two time points (default in seconds)
     */
    template<typename T = DefaultResolution>
    double tick() {
        auto now = Clock::now();
        auto duration = std::chrono::duration<double, T>(now - _previousTick);
        _previousTick = now;
        return duration.count();
    }
    
    /**
     * @brief Check if the timer is running
     */
    bool isRunning() const;
    
private:
    bool _running{false};
    
    bool _lapping{false};
    
    Clock::time_point _startTime;
    
    Clock::time_point _lapTime;
    
    Clock::time_point _previousTick;
};

}        // namespace vox
