//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/animation/animator.h"

namespace vox {
void Animator::update(float dt) {
    if (blending) {
        blending->update(dt);
    }
}
}  // namespace vox