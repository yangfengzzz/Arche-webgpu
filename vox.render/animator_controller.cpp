//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "animator_controller.h"
#include "math_utils.h"

namespace vox {
AnimatorController::AnimatorController()
: _timeRatio(0.f),
_previousTimeRatio(0.f),
_playbackSpeed(1.f),
_play(true),
_loop(true) {
}

void AnimatorController::update(const ozz::animation::Animation &_animation,
                                float _dt) {
    float new_time = _timeRatio;
    
    if (_play) {
        new_time = _timeRatio + _dt * _playbackSpeed / _animation.duration();
    }
    
    // Must be called even if time doesn't change, in order to update previous
    // frame time ratio. Uses set_time_ratio function in order to update
    // previous_time_ an wrap time value in the unit interval (depending on loop
    // mode).
    setTimeRatio(new_time);
}

void AnimatorController::setTimeRatio(float _ratio) {
    _previousTimeRatio = _timeRatio;
    if (_loop) {
        // Wraps in the unit interval [0:1], even for negative values (the reason
        // for using floorf).
        _timeRatio = _ratio - floorf(_ratio);
    } else {
        // Clamps in the unit interval [0:1].
        _timeRatio = clamp(0.f, _ratio, 1.f);
    }
}

// Gets animation current time.
float AnimatorController::timeRatio() const {
    return _timeRatio;
}

// Gets animation time of last update.
float AnimatorController::previousTimeRatio() const {
    return _previousTimeRatio;
}

void AnimatorController::reset() {
    _previousTimeRatio = _timeRatio = 0.f;
    _playbackSpeed = 1.f;
    _play = true;
}

}
