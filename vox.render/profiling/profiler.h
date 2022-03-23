//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef profiler_hpp
#define profiler_hpp

#include "profiler_report.h"
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <thread>

namespace vox {
/* Forward declaration of the profiler spy structure */
struct ProfilerSpy;

/**
 * The profiler collect data about the running program
 */
class Profiler final {
public:
    /**
     * Create the profiler
     */
    Profiler();
    
    /**
     * Generate a report containing data about the last profiling session
     */
    ProfilerReport generateReport();
    
    /**
     * Clear any collected data
     */
    void clearHistory();
    
    /**
     * Update the profiler
     */
    void update(float p_deltaTime);
    
    /**
     * Save the given spy collected data to the profiler history
     * @param p_spy (Spy to collect data from)
     */
    static void save(ProfilerSpy &p_spy);
    
    /**
     * Verify if the profiler is currently enabled
     */
    static bool isEnabled();
    
    /**
     * Enable the profiler if it is currently disabled, and vice-versa
     */
    static void toggleEnable();
    
    /**
     * Enable the profiler
     */
    static void enable();
    
    /**
     * Disable the profiler
     */
    static void disable();
    
private:
    /* Time relatives */
    std::chrono::steady_clock::time_point _lastTime;
    
    /* Profiler settings */
    static bool __ENABLED;
    
    /* Collected data */
    static std::mutex __SAVE_MUTEX;
    static std::unordered_map<std::string, double> __ELPASED_HISTORY;
    static std::unordered_map<std::string, uint64_t> __CALLS_COUNTER;
    static std::vector<std::thread::id> __WORKING_THREADS;
    static uint32_t __ELAPSED_FRAMES;
};

}

#endif /* profiler_hpp */
