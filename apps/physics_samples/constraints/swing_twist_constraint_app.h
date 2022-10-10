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
class PhysicsSwingTwistConstraintApp : public ForwardApplication {
public:
    bool prepare(Platform &platform) override;

    void loadScene() override;

private:
    JPH::Array<JPH::Ref<JPH::SwingTwistConstraint>> mConstraints;

    inline static float sNormalHalfConeAngle = degreesToRadians(60);
    inline static float sPlaneHalfConeAngle = degreesToRadians(20);
    inline static float sTwistMinAngle = degreesToRadians(-10);
    inline static float sTwistMaxAngle = degreesToRadians(20);
};
}  // namespace vox