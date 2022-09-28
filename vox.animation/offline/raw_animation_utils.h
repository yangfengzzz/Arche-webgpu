//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.animation/offline/export.h"
#include "vox.animation/offline/raw_animation.h"
#include "vox.base/span.h"
#include "vox.math/scalable_transform.h"

namespace vox::animation::offline {

// Translation interpolation method.
VOX_ANIMOFFLINE_DLL Vector3F LerpTranslation(const Vector3F& _a, const Vector3F& _b, float _alpha);

// Rotation interpolation method.
VOX_ANIMOFFLINE_DLL QuaternionF LerpRotation(const QuaternionF& _a, const QuaternionF& _b, float _alpha);

// Scale interpolation method.
VOX_ANIMOFFLINE_DLL Vector3F LerpScale(const Vector3F& _a, const Vector3F& _b, float _alpha);

// Samples a RawAnimation track. This function shall be used for offline
// purpose. Use vox::animation::Animation and vox::animation::SamplingJob for
// runtime purpose.
// Returns false if track is invalid.
VOX_ANIMOFFLINE_DLL bool SampleTrack(const RawAnimation::JointTrack& _track,
                                     float _time,
                                     vox::ScalableTransform* _transform);

// Samples a RawAnimation. This function shall be used for offline
// purpose. Use vox::animation::Animation and vox::animation::SamplingJob for
// runtime purpose.
// _animation must be valid.
// Returns false output range is too small or animation is invalid.
VOX_ANIMOFFLINE_DLL bool SampleAnimation(const RawAnimation& _animation,
                                         float _time,
                                         const span<vox::ScalableTransform>& _transforms);

// Implement fixed rate keyframe time iteration. This utility purpose is to
// ensure that sampling goes strictly from 0 to duration, and that period
// between consecutive time samples have a fixed period.
// This sounds trivial, but floating point error could occur if keyframe time
// was accumulated for a long duration.
class VOX_ANIMOFFLINE_DLL FixedRateSamplingTime {
public:
    FixedRateSamplingTime(float _duration, float _frequency);

    [[nodiscard]] float time(size_t _key) const {
        assert(_key < num_keys_);
        return std::min(_key * period_, duration_);
    }

    [[nodiscard]] size_t num_keys() const { return num_keys_; }

private:
    float duration_;
    float period_;
    size_t num_keys_;
};
}  // namespace vox::animation::offline