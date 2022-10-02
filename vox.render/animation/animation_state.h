//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.animation/runtime/skeleton.h"
#include "vox.base/containers/vector.h"
#include "vox.simd_math/soa_transform.h"

namespace vox {
class AnimationState {
public:
    enum BlendMode { Normal, Additive, NoBlend };
    BlendMode blendMode = BlendMode::Normal;

    float weight{0.f};

    vox::vector<vox::simd_math::SimdFloat4>& jointMasks();

    void setJointMasks(float mask, const char* root = nullptr);

    void addChild(const std::shared_ptr<AnimationState>& state);

    void removeChild(const std::shared_ptr<AnimationState>& state);

public:
    virtual void update(float dt) = 0;

    [[nodiscard]] virtual const vox::vector<simd_math::SoaTransform>& locals() const = 0;

    virtual void loadSkeleton(animation::Skeleton* skeleton) { _skeleton = skeleton; }

protected:
    animation::Skeleton* _skeleton{nullptr};

    // Per-joint weights used to define the partial animation mask. Allows to
    // select which joints are considered during blending, and their individual
    // weight_setting.
    vox::vector<vox::simd_math::SimdFloat4> _joint_masks;

    vox::vector<std::shared_ptr<AnimationState>> _states{};
};
}  // namespace vox