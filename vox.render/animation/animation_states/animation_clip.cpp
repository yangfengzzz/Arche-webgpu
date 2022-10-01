//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/animation/animation_states/animation_clip.h"

#include "vox.animation/runtime/skeleton_utils.h"
#include "vox.base/io/archive.h"
#include "vox.base/logging.h"
#include "vox.render/platform/filesystem.h"

namespace vox {
AnimationClip::AnimationClip(const std::string& filename) {
    if (!filename.empty()) {
        loadAnimation(filename);
    }
    _sampling_job.animation = &_animation;
    _sampling_job.context = &_context;
}

AnimationClip::AnimationClip(AnimationClip&& state) noexcept
    : _time_ratio(state._time_ratio), _animation(std::move(state._animation)) {
    _sampling_job.animation = &_animation;
}

AnimationClip& AnimationClip::operator=(AnimationClip&& state) noexcept {
    _time_ratio = state._time_ratio;
    _animation = std::move(state._animation);
    _sampling_job.animation = &_animation;
    return *this;
}

vox::vector<vox::simd_math::SimdFloat4>& AnimationClip::jointMasks() { return _joint_masks; }

void AnimationClip::setJointMasks(float mask, const std::string& root) {
    simd_math::SimdFloat4 simdMask = simd_math::simd_float4::Load1(mask);
    if (root.empty()) {
        for (int i = 0; i < _skeleton->num_soa_joints(); ++i) {
            _joint_masks[i] = simdMask;
        }
    } else {
        const auto set_joint = [this, simdMask](int _joint, int) {
            simd_math::SimdFloat4& soa_weight = _joint_masks[_joint / 4];
            soa_weight = simd_math::SetI(soa_weight, simdMask, _joint % 4);
        };

        const int joint = FindJoint(*_skeleton, root.c_str());
        if (joint >= 0) {
            animation::IterateJointsDF(*_skeleton, set_joint, joint);
        }
    }
}

animation::Animation& AnimationClip::animation() { return _animation; }

bool AnimationClip::loadAnimation(const std::string& filename) {
    LOGI("Loading animation archive: {}", filename)
    vox::io::File file((fs::path::Get(fs::path::Type::ASSETS) + filename).c_str(), "rb");
    if (!file.opened()) {
        LOGE("Failed to open animation file {}", filename)
        return false;
    }
    vox::io::IArchive archive(&file);
    if (!archive.TestTag<vox::animation::Animation>()) {
        LOGE("Failed to load animation instance from file {}.", filename)
        return false;
    }

    // Once the tag is validated, reading cannot fail.
    archive >> _animation;

    return true;
}

const vox::vector<simd_math::SoaTransform>& AnimationClip::locals() const { return _locals; }

void AnimationClip::loadSkeleton(animation::Skeleton* skeleton) {
    _skeleton = skeleton;
    _context.Resize(skeleton->num_joints());
    _locals.resize(skeleton->num_soa_joints());
    _sampling_job.output = make_span(_locals);
}

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
        (void)_sampling_job.Run();
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