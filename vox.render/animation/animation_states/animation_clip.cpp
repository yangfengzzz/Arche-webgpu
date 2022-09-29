//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/animation/animation_states/animation_clip.h"

#include "vox.base/io/archive.h"
#include "vox.base/logging.h"
#include "vox.math/math_utils.h"

namespace vox {
AnimationClip::AnimationClip(const char* _filename) {
    bool flag = loadAnimation(_filename);
    if (flag) {
        _sampling_job.animation = &_animation;
        _sampling_job.context = &_context;
    }
}

AnimationClip::AnimationClip(AnimationClip&& state) noexcept
    : _time_ratio(state._time_ratio), _animation(std::move(state._animation)) {
    _sampling_job.animation = &_animation;
}

AnimationClip& AnimationClip::operator=(AnimationClip&&) noexcept {}

bool AnimationClip::loadAnimation(const char* _filename) {
    assert(_filename);
    LOGI("Loading animation archive: {}", _filename)
    vox::io::File file(_filename, "rb");
    if (!file.opened()) {
        LOGE("Failed to open animation file {}", _filename)
        return false;
    }
    vox::io::IArchive archive(&file);
    if (!archive.TestTag<vox::animation::Animation>()) {
        LOGE("Failed to load animation instance from file {}.", _filename)
        return false;
    }

    // Once the tag is validated, reading cannot fail.
    archive >> _animation;

    return true;
}

const vox::vector<simd_math::SoaTransform>& AnimationClip::locals() const { return _locals; }

void AnimationClip::_setNumSoaJoints(int value) {
    _locals.resize(value);
    _sampling_job.output = make_span(_locals);
}

void AnimationClip::_setNumJoints(int value) { _context.Resize(value); }

void AnimationClip::update(float dt) {
    float new_time = _time_ratio;

    if (play) {
        new_time = _time_ratio + dt * playback_speed / _animation.duration();
    }

    // Must be called even if time doesn't change, in order to update previous
    // frame time ratio. Uses set_time_ratio function in order to update
    // previous_time_ a wrap time value in the unit interval (depending on loop
    // mode).
    setTimeRatio(new_time);
    _sampling_job.ratio = timeRatio();
    if (_sampling_job.animation) {
        _sampling_job.Run();
    }
}

void AnimationClip::setTimeRatio(float _ratio) {
    _previous_time_ratio = _time_ratio;
    if (loop) {
        // Wraps in the unit interval [0:1], even for negative values (the reason
        // for using floorf).
        _time_ratio = _ratio - floorf(_ratio);
    } else {
        // Clamps in the unit interval [0:1].
        _time_ratio = vox::clamp(0.f, _ratio, 1.f);
    }
}

float AnimationClip::timeRatio() const { return _time_ratio; }

float AnimationClip::previousTimeRatio() const { return _previous_time_ratio; }

void AnimationClip::reset() {
    _previous_time_ratio = _time_ratio = 0.f;
    playback_speed = 1.f;
    play = true;
}

}  // namespace vox