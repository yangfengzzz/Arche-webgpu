//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/component.h"
#include "vox.render/animation/animator_controller.h"

namespace vox {
class Animator : public Component {
public:
    AnimatorController controller;

    void update();
};
}
