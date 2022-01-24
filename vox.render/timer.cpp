//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "timer.h"

namespace vox {
Timer::Timer() :
_startTime{Clock::now()},
_previousTick{Clock::now()} {
}

void Timer::start() {
    if (!_running) {
        _running = true;
        _startTime = Clock::now();
    }
}

void Timer::lap() {
    _lapping = true;
    _lapTime = Clock::now();
}

bool Timer::isRunning() const {
    return _running;
}

}        // namespace vox
