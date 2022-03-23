//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "profiler.h"
#include "profiler_spy.h"
#include <iostream>
#include <iomanip>
#include <map>
#include <string>

namespace vox {
bool Profiler::__ENABLED;
std::mutex Profiler::__SAVE_MUTEX;
std::unordered_map<std::string, double> Profiler::__ELPASED_HISTORY;
std::unordered_map<std::string, uint64_t> Profiler::__CALLS_COUNTER;
std::vector<std::thread::id> Profiler::__WORKING_THREADS;
uint32_t Profiler::__ELAPSED_FRAMES;

Profiler::Profiler() {
    _lastTime = std::chrono::high_resolution_clock::now();
    __ENABLED = false;
}

ProfilerReport Profiler::generateReport() {
    ProfilerReport report;
    
    if (__ELAPSED_FRAMES == 0)
        return report;
    
    std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - _lastTime;
    
    report.workingThreads = static_cast<uint16_t>((__WORKING_THREADS.size() - 1) / __ELAPSED_FRAMES);
    report.elapsedFrames = __ELAPSED_FRAMES;
    report.elaspedTime = elapsed.count();
    
    std::multimap<double, std::string, std::greater<double>> sortedHistory;
    
    /* Fill the sorted history with the current history (Auto sort) */
    for (auto &data: __ELPASED_HISTORY)
        sortedHistory.insert(std::pair<double, std::string>(data.second, data.first));
    
    /* Add every actions to the report */
    for (auto &data: sortedHistory)
        report.actions.push_back({data.second, data.first, (data.first / elapsed.count()) * 100.0, __CALLS_COUNTER[data.second]});
    
    return report;
}

void Profiler::clearHistory() {
    __ELPASED_HISTORY.clear();
    __CALLS_COUNTER.clear();
    __WORKING_THREADS.clear();
    __ELAPSED_FRAMES = 0;
    
    _lastTime = std::chrono::high_resolution_clock::now();
}

void Profiler::update(float p_deltaTime) {
    if (isEnabled()) {
        ++__ELAPSED_FRAMES;
    }
}

void Profiler::save(ProfilerSpy &p_spy) {
    __SAVE_MUTEX.lock();
    
    /* Check if this thread is already registered */
    if (std::find(__WORKING_THREADS.begin(), __WORKING_THREADS.end(), std::this_thread::get_id()) == __WORKING_THREADS.end())
        __WORKING_THREADS.push_back(std::this_thread::get_id());
    
    if (__ELPASED_HISTORY.find(p_spy.name) != __ELPASED_HISTORY.end()) {
        __ELPASED_HISTORY[p_spy.name] += std::chrono::duration<double>(p_spy.end - p_spy.start).count();
    } else {
        __ELPASED_HISTORY[p_spy.name] = std::chrono::duration<double>(p_spy.end - p_spy.start).count();
    }
    
    if (__CALLS_COUNTER.find(p_spy.name) != __CALLS_COUNTER.end()) {
        ++__CALLS_COUNTER[p_spy.name];
    } else {
        __CALLS_COUNTER[p_spy.name] = 1;
    }
    
    __SAVE_MUTEX.unlock();
}

bool Profiler::isEnabled() {
    return __ENABLED;
}

void Profiler::toggleEnable() {
    __ENABLED = !__ENABLED;
}

void Profiler::enable() {
    __ENABLED = true;
}

void Profiler::disable() {
    __ENABLED = false;
}

}
