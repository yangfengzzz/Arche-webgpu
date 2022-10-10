//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/forward_application.h"
//
#include <Jolt/Physics/Constraints/PathConstraint.h>

namespace vox {
class PhysicsPathConstraintApp : public ForwardApplication {
public:
    bool prepare(Platform &platform) override;

    void loadScene() override;

private:
    JPH::Ref<JPH::PathConstraintPath> mPaths[2];
    JPH::Ref<JPH::PathConstraint> mConstraints[2];

    inline static float sMaxMotorAcceleration = 20.0f;
    inline static float sMaxFrictionAcceleration = 0.0f;
    inline static float sFrequency = 2.0f;
    inline static float sDamping = 1.0f;
    ///< The orientation constraint type for the path constraint
    static JPH::EPathRotationConstraintType sOrientationType;
};
}  // namespace vox