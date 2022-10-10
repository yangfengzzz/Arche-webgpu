//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/forward_application.h"
//
#include <Jolt/Physics/Constraints/SixDOFConstraint.h>

namespace vox {
class PhysicsSixDofConstraintApp : public ForwardApplication {
public:
    bool prepare(Platform &platform) override;

    void loadScene() override;

private:
    using SettingsRef = JPH::Ref<JPH::SixDOFConstraintSettings>;
    using EAxis = JPH::SixDOFConstraintSettings::EAxis;

    static float sLimitMin[EAxis::Num];
    static float sLimitMax[EAxis::Num];
    static bool sEnableLimits[EAxis::Num];
    static SettingsRef sSettings;

    JPH::Vec3 mTargetOrientationCS = JPH::Vec3::sZero();

    JPH::Ref<JPH::SixDOFConstraint> mConstraint;
};
}  // namespace vox