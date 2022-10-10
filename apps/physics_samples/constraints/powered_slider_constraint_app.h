//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/forward_application.h"
//
#include <Jolt/Physics/Constraints/SliderConstraint.h>

namespace vox {
class PhysicsPoweredSliderConstraintApp : public ForwardApplication {
public:
    bool prepare(Platform &platform) override;

    void loadScene() override;

private:
    inline static float sMaxMotorAcceleration = 250.0f;
    inline static float sMaxFrictionAcceleration = 0.0f;
    inline static float sFrequency = 2.0f;
    inline static float sDamping = 1.0f;

    JPH::Body *mBody2 = nullptr;
    JPH::SliderConstraint *mConstraint = nullptr;
};
}  // namespace vox