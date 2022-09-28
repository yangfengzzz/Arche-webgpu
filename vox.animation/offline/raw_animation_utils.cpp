//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.animation/offline/raw_animation_utils.h"

#include <algorithm>
#include <limits>

namespace vox::animation::offline {

// Translation interpolation method.
// This must be the same Lerp as the one used by the sampling job.
Vector3F LerpTranslation(const Vector3F& _a, const Vector3F& _b, float _alpha) { return vox::lerp(_a, _b, _alpha); }

// Rotation interpolation method.
// This must be the same Lerp as the one used by the sampling job.
// The goal is to take the shortest path between _a and _b. This code replicates
// this behavior that is actually not done at runtime, but when building the
// animation.
QuaternionF LerpRotation(const QuaternionF& _a, const QuaternionF& _b, float _alpha) {
    // Finds the shortest path. This is done by the AnimationBuilder for runtime
    // animations.
    const float dot = _a.x * _b.x + _a.y * _b.y + _a.z * _b.z + _a.w * _b.w;
    return vox::nLerp(_a, dot < 0.f ? -_b : _b, _alpha);  // _b an -_b are the
                                                          // same rotation.
}

// Scale interpolation method.
// This must be the same Lerp as the one used by the sampling job.
Vector3F LerpScale(const Vector3F& _a, const Vector3F& _b, float _alpha) { return vox::lerp(_a, _b, _alpha); }

namespace {

// The next functions are used to sample a RawAnimation. This feature is not
// part of vox sdk, as RawAnimation is an intermediate format used to build the
// runtime animation.

// Less comparator, used by search algorithm to walk through track sorted
// keyframes
template <typename Key>
bool Less(const Key& _left, const Key& _right) {
    return _left.time < _right.time;
}

// Samples a component (translation, rotation or scale) of a track.
template <typename Track, typename Lerp>
typename Track::value_type::Value SampleComponent(const Track& _track, const Lerp& _lerp, float _time) {
    if (_track.size() == 0) {
        // Return identity if there's no key for this track.
        return Track::value_type::identity();
    } else if (_time <= _track.front().time) {
        // Returns the first keyframe if _time is before the first keyframe.
        return _track.front().value;
    } else if (_time >= _track.back().time) {
        // Returns the last keyframe if _time is before the last keyframe.
        return _track.back().value;
    } else {
        // Needs to interpolate the 2 keyframes before and after _time.
        assert(_track.size() >= 2);
        // First find the 2 keys.
        const typename Track::value_type cmp = {_time, Track::value_type::identity()};
        auto it = std::lower_bound(array_begin(_track), array_end(_track), cmp, Less<typename Track::value_type>);
        assert(it > array_begin(_track) && it < array_end(_track));

        // Then interpolate them at t = _time.
        const typename Track::const_reference right = it[0];
        const typename Track::const_reference left = it[-1];
        const float alpha = (_time - left.time) / (right.time - left.time);
        return _lerp(left.value, right.value, alpha);
    }
}

void SampleTrack_NoValidate(const RawAnimation::JointTrack& _track, float _time, vox::ScalableTransform* _transform) {
    _transform->translation = SampleComponent(_track.translations, LerpTranslation, _time);
    _transform->rotation = SampleComponent(_track.rotations, LerpRotation, _time);
    _transform->scale = SampleComponent(_track.scales, LerpScale, _time);
}
}  // namespace

bool SampleTrack(const RawAnimation::JointTrack& _track, float _time, vox::ScalableTransform* _transform) {
    if (!_track.Validate(std::numeric_limits<float>::infinity())) {
        return false;
    }

    SampleTrack_NoValidate(_track, _time, _transform);
    return true;
}

bool SampleAnimation(const RawAnimation& _animation, float _time, const span<vox::ScalableTransform>& _transforms) {
    if (!_animation.Validate()) {
        return false;
    }
    if (_animation.tracks.size() > _transforms.size()) {
        return false;
    }

    for (size_t i = 0; i < _animation.tracks.size(); ++i) {
        SampleTrack_NoValidate(_animation.tracks[i], _time, _transforms.begin() + i);
    }
    return true;
}

FixedRateSamplingTime::FixedRateSamplingTime(float _duration, float _frequency)
    : duration_(_duration),
      period_(1.f / _frequency),
      num_keys_(static_cast<size_t>(std::ceil(1.f + _duration * _frequency))) {}

}  // namespace vox::animation::offline
