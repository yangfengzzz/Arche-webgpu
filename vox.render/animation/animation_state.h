//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/containers/vector.h"
#include "vox.simd_math/soa_transform.h"

namespace vox {
class AnimationState {
public:
    virtual void update(float dt) = 0;

    [[nodiscard]] virtual const vox::vector<simd_math::SoaTransform>& locals() const = 0;
};
}