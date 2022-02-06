//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "fixed_joint.h"
#include "../physics_manager.h"
#include "../collider.h"

namespace vox {
namespace physics {
FixedJoint::FixedJoint(Collider *collider0, Collider *collider1) {
    auto actor0 = collider0 ? collider0->handle() : nullptr;
    auto actor1 = collider1 ? collider1->handle() : nullptr;
    _nativeJoint = PxFixedJointCreate(*PhysicsManager::_nativePhysics(),
                                      actor0, PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)),
                                      actor1, PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)));
}

void FixedJoint::setProjectionLinearTolerance(float tolerance) {
    return static_cast<PxFixedJoint *>(_nativeJoint)->setProjectionLinearTolerance(tolerance);
}

float FixedJoint::projectionLinearTolerance() const {
    return static_cast<PxFixedJoint *>(_nativeJoint)->getProjectionLinearTolerance();
}

void FixedJoint::setProjectionAngularTolerance(float tolerance) {
    return static_cast<PxFixedJoint *>(_nativeJoint)->setProjectionAngularTolerance(tolerance);
}

float FixedJoint::projectionAngularTolerance() const {
    return static_cast<PxFixedJoint *>(_nativeJoint)->getProjectionAngularTolerance();
}

}
}
