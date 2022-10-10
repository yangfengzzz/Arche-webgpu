//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/forward_application.h"
//
#include <Jolt/Physics/Constraints/SwingTwistConstraint.h>

namespace vox {
class PhysicsPoweredSwingTwistConstraintApp : public ForwardApplication {
public:
    bool prepare(Platform &platform) override;

    void loadScene() override;

private:
    static JPH::Vec3 sBodyRotation[2];
    inline static JPH::EMotorState sSwingMotorState = JPH::EMotorState::Velocity;
    inline static JPH::EMotorState sTwistMotorState = JPH::EMotorState::Velocity;
    inline static JPH::Vec3 sTargetVelocityCS = JPH::Vec3(degreesToRadians(90), 0, 0);
    inline static JPH::Vec3 sTargetOrientationCS = JPH::Vec3::sZero();
    inline static float sMaxAngularAcceleration = degreesToRadians(36000.0f);
    inline static float sMaxFrictionAngularAcceleration = 0.0f;
    inline static float sNormalHalfConeAngle = degreesToRadians(60);
    inline static float sPlaneHalfConeAngle = degreesToRadians(45);
    inline static float sTwistMinAngle = degreesToRadians(-180);
    inline static float sTwistMaxAngle = degreesToRadians(180);
    inline static float sFrequency = 10.0f;
    inline static float sDamping = 2.0f;

    JPH::SwingTwistConstraint *mConstraint = nullptr;
    float mInertiaBody2AsSeenFromConstraint;
};
}  // namespace vox