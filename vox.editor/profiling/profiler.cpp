//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/profiling/profiler.h"

#include <map>

#include "vox.editor/profiling/profiler_spy.h"

namespace vox {
bool Profiler::enabled_;
std::mutex Profiler::save_mutex_;
std::unordered_map<std::string, double> Profiler::elapsed_history_;
std::unordered_map<std::string, uint64_t> Profiler::calls_counter_;
std::vector<std::thread::id> Profiler::working_threads_;
uint32_t Profiler::elapsed_frames_;

Profiler::Profiler() {
    last_time_ = std::chrono::high_resolution_clock::now();
    enabled_ = false;
}

ProfilerReport Profiler::GenerateReport() {
    ProfilerReport report;

    if (elapsed_frames_ == 0) return report;

    std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - last_time_;

    report.working_threads = static_cast<uint16_t>((working_threads_.size() - 1) / elapsed_frames_);
    report.elapsed_frames = elapsed_frames_;
    report.elapsed_time = elapsed.count();

    std::multimap<double, std::string, std::greater<>> sorted_history;

    /* Fill the sorted history with the current history (Auto sort) */
    for (auto &data : elapsed_history_) sorted_history.insert(std::pair<double, std::string>(data.second, data.first));

    /* Add every action to the report */
    for (auto &data : sorted_history)
        report.actions.push_back(
                {data.second, data.first, (data.first / elapsed.count()) * 100.0, calls_counter_[data.second]});

    return report;
}

void Profiler::ClearHistory() {
    elapsed_history_.clear();
    calls_counter_.clear();
    working_threads_.clear();
    elapsed_frames_ = 0;

    last_time_ = std::chrono::high_resolution_clock::now();
}

void Profiler::Update(float delta_time) {
    if (IsEnabled()) {
        ++elapsed_frames_;
    }
}

void Profiler::Save(ProfilerSpy &spy) {
    save_mutex_.lock();

    /* Check if this thread is already registered */
    if (std::find(working_threads_.begin(), working_threads_.end(), std::this_thread::get_id()) ==
        working_threads_.end())
        working_threads_.push_back(std::this_thread::get_id());

    if (elapsed_history_.find(spy.name) != elapsed_history_.end()) {
        elapsed_history_[spy.name] += std::chrono::duration<double>(spy.end - spy.start).count();
    } else {
        elapsed_history_[spy.name] = std::chrono::duration<double>(spy.end - spy.start).count();
    }

    if (calls_counter_.find(spy.name) != calls_counter_.end()) {
        ++calls_counter_[spy.name];
    } else {
        calls_counter_[spy.name] = 1;
    }

    save_mutex_.unlock();
}

bool Profiler::IsEnabled() { return enabled_; }

void Profiler::ToggleEnable() { enabled_ = !enabled_; }

void Profiler::Enable() { enabled_ = true; }

void Profiler::Disable() { enabled_ = false; }

}  // namespace vox
