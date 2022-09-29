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
class AnimationCrossFade : public AnimationState {
public:
    void loadSkeleton(const animation::Skeleton& skeleton) override;

    void update(float dt) override;

    [[nodiscard]] const vox::vector<simd_math::SoaTransform>& locals() const override;

private:
    int num_soa_joints{};
    int num_joints{};
    vox::vector<AnimationClip> _clips;

    animation::BlendingJob _blend_job;

    vox::vector<animation::BlendingJob::Layer> _additive_layers;
    vox::vector<animation::BlendingJob::Layer> _layers;
    // Buffer of local transforms which stores the blending result.
    vox::vector<simd_math::SoaTransform> _blended_locals;
};
}  // namespace vox
