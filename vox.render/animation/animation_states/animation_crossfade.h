//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/animation/animation_state.h"

namespace vox {
class AnimationCrossFade : public AnimationState {
public:
    void update(float dt) override;

    [[nodiscard]] const vox::vector<simd_math::SoaTransform>& locals() const override;
};
}  // namespace vox
