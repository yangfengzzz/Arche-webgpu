//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "spherical_joint.h"
#include "../physics_manager.h"
#include "../collider.h"

namespace vox {
namespace physics {
SphericalJoint::SphericalJoint(Collider *collider0, Collider *collider1) {
    auto actor0 = collider0 ? collider0->handle() : nullptr;
    auto actor1 = collider1 ? collider1->handle() : nullptr;
    _nativeJoint = PxSphericalJointCreate(*PhysicsManager::_nativePhysics(),
                                          actor0, PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)),
                                          actor1, PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)));
}

PxJointLimitCone SphericalJoint::limitCone() const {
    return static_cast<PxSphericalJoint *>(_nativeJoint)->getLimitCone();
}

void SphericalJoint::setLimitCone(const PxJointLimitCone &limit) {
    static_cast<PxSphericalJoint *>(_nativeJoint)->setLimitCone(limit);
}

float SphericalJoint::swingYAngle() const {
    return static_cast<PxSphericalJoint *>(_nativeJoint)->getSwingYAngle();
}

float SphericalJoint::swingZAngle() const {
    return static_cast<PxSphericalJoint *>(_nativeJoint)->getSwingZAngle();
}

void SphericalJoint::setSphericalJointFlags(PxSphericalJointFlags flags) {
    static_cast<PxSphericalJoint *>(_nativeJoint)->setSphericalJointFlags(flags);
}

void SphericalJoint::setSphericalJointFlag(PxSphericalJointFlag::Enum flag, bool value) {
    static_cast<PxSphericalJoint *>(_nativeJoint)->setSphericalJointFlag(flag, value);
}

PxSphericalJointFlags SphericalJoint::sphericalJointFlags() const {
    return static_cast<PxSphericalJoint *>(_nativeJoint)->getSphericalJointFlags();
}

void SphericalJoint::setProjectionLinearTolerance(float tolerance) {
    static_cast<PxSphericalJoint *>(_nativeJoint)->setProjectionLinearTolerance(tolerance);
}

float SphericalJoint::projectionLinearTolerance() const {
    return static_cast<PxSphericalJoint *>(_nativeJoint)->getProjectionLinearTolerance();
}


}
}
