//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.base/timer.h"

#include <chrono>
#include <utility>

#include "vox.base/logging.h"

namespace vox::utility {

Timer::Timer() : start_time_{Clock::now()}, previous_tick_{Clock::now()} {}

void Timer::Start() {
    if (!running_) {
        running_ = true;
        start_time_ = Clock::now();
    }
}

void Timer::Lap() {
    lapping_ = true;
    lap_time_ = Clock::now();
}

bool Timer::IsRunning() const { return running_; }

// MARK: - ScopeTimer
ScopeTimer::ScopeTimer(std::string scope_timer_info /* = ""*/) : scope_timer_info_(std::move(scope_timer_info)) {
    Timer::Start();
}

ScopeTimer::~ScopeTimer() {
    auto duration = Timer::Stop();
    LOGI("{} {:.2f} ms.", scope_timer_info_ + " took", duration)
}

// MARK: - FPSTimer
FPSTimer::FPSTimer(std::string fps_timer_info /* = ""*/,
                   int expectation /* = -1*/,
                   double time_to_print /* = 3000.0*/,
                   int events_to_print /* = 100*/)
    : fps_timer_info_(std::move(fps_timer_info)),
      expectation_(expectation),
      time_to_print_(time_to_print),
      events_to_print_(events_to_print),
      event_fragment_count_(0),
      event_total_count_(0) {
    Start();
}

void FPSTimer::Signal() {
    event_fragment_count_++;
    event_total_count_++;
    auto duration = Stop();
    if (duration >= time_to_print_ || event_fragment_count_ >= events_to_print_) {
        // print and reset
        if (expectation_ == -1) {
            LOGI("{} at {:.2f} fps.", fps_timer_info_, double(event_fragment_count_ + 1) / duration * 1000.0)
        } else {
            LOGI("{} at {:.2f} fps (progress {:.2f}%).", fps_timer_info_.c_str(),
                 double(event_fragment_count_ + 1) / duration * 1000.0,
                 (double)event_total_count_ * 100.0 / (double)expectation_)
        }
        Start();
        event_fragment_count_ = 0;
    }
}

}  // namespace vox::utility
