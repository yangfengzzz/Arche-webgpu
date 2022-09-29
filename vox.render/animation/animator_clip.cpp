//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.base/io/archive.h"
#include "vox.base/logging.h"
#include "vox.math/math_utils.h"
#include "vox.render/animation/animator_clip.h"
#include "vox.simd_math/soa_transform.h"

namespace vox {
AnimatorClip::AnimatorClip(const char* _filename) {
    bool flag = loadAnimation(_filename);
    if (flag) {
        _sampling_job.animation = &_animation;
        _sampling_job.context = &_context;
    }
}

AnimatorClip::AnimatorClip(AnimatorClip&& state) noexcept
    : _time_ratio(state._time_ratio),
      _animation(std::move(state._animation)) {
    _sampling_job.animation = &_animation;
}

AnimatorClip& AnimatorClip::operator=(AnimatorClip&&) noexcept {}

bool AnimatorClip::loadAnimation(const char* _filename) {
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

const vox::vector<simd_math::SoaTransform>& AnimatorClip::locals() const {
    return _locals;
}

void AnimatorClip::_setNumSoaJoints(int value) {
    _locals.resize(value);
    _sampling_job.output = make_span(_locals);
}

void AnimatorClip::_setNumJoints(int value) {
    _context.Resize(value);
}

void AnimatorClip::update(float dt) {
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

void AnimatorClip::setTimeRatio(float _ratio) {
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

float AnimatorClip::timeRatio() const { return _time_ratio; }

float AnimatorClip::previousTimeRatio() const { return _previous_time_ratio; }

void AnimatorClip::reset() {
    _previous_time_ratio = _time_ratio = 0.f;
    playback_speed = 1.f;
    play = true;
}

}  // namespace vox