//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/forward_application.h"
//
#include <Jolt/Physics/Constraints/HingeConstraint.h>

namespace vox {
class PhysicsPowerHingeConstraintApp : public ForwardApplication {
public:
    bool prepare(Platform &platform) override;

    void loadScene() override;

private:
    inline static float sMaxAngularAcceleration = degreesToRadians(3600.0f);
    inline static float sMaxFrictionAngularAcceleration = 0.0f;
    inline static float sFrequency = 2.0f;
    inline static float sDamping = 1.0f;

    JPH::HingeConstraint *mConstraint = nullptr;
    float mInertiaBody2AsSeenFromConstraint;
};
}  // namespace vox