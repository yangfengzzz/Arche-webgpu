//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/animation/animator_clip.h"
#include "vox.animation/runtime/blending_job.h"
#include "vox.animation/runtime/skeleton.h"

namespace vox {
class AnimatorBlending {
public:
    explicit AnimatorBlending(const animation::Skeleton& skeleton);

    AnimatorClip& addAnimatorClip(const char* _filename);

    void update(float dt);

    [[nodiscard]] float threshold() const;

    void setThreshold(float value);

private:
    // Runtime skeleton.
    const animation::Skeleton& _skeleton;

    vox::vector<AnimatorClip> _clips;

    animation::BlendingJob _blend_job;

    // Buffer of local transforms which stores the blending result.
    vox::vector<simd_math::SoaTransform> _blended_locals;
};
}  // namespace vox
