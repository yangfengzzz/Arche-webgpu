//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.animation/runtime/blending_job.h"
#include "vox.animation/runtime/skeleton.h"
#include "vox.render/animation/animation_state.h"
#include "vox.render/animation/animation_states/animation_clip.h"

namespace vox {
class AnimatorBlending : public AnimationState {
public:
    float blendRatio{0.f};

    [[nodiscard]] float threshold() const;

    void setThreshold(float value);

    std::shared_ptr<AnimationClip> addAnimatorClip(const std::string& filename, float location);

    void loadSkeleton(animation::Skeleton* skeleton) override;

    void update(float dt) override;

    [[nodiscard]] const vox::vector<simd_math::SoaTransform>& locals() const override;

private:
    animation::Skeleton* _skeleton{nullptr};
    vox::vector<std::shared_ptr<AnimationClip>> _clips{};
    vox::vector<float> _locations{};
    animation::BlendingJob _blend_job;

    vox::vector<animation::BlendingJob::Layer> _additive_layers;
    vox::vector<animation::BlendingJob::Layer> _layers;
    // Buffer of local transforms which stores the blending result.
    vox::vector<simd_math::SoaTransform> _blended_locals;
};
}  // namespace vox
