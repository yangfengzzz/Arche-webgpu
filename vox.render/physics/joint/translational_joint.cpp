//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "translational_joint.h"
#include "../physics_manager.h"
#include "../collider.h"

namespace vox {
namespace physics {
TranslationalJoint::TranslationalJoint(Collider *collider0, Collider *collider1) {
    auto actor0 = collider0 ? collider0->handle() : nullptr;
    auto actor1 = collider1 ? collider1->handle() : nullptr;
    _nativeJoint = PxPrismaticJointCreate(*PhysicsManager::_nativePhysics(),
                                          actor0, PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)),
                                          actor1, PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)));
}

float TranslationalJoint::position() const {
    return static_cast<PxPrismaticJoint *>(_nativeJoint)->getPosition();
}

float TranslationalJoint::velocity() const {
    return static_cast<PxPrismaticJoint *>(_nativeJoint)->getVelocity();
}

void TranslationalJoint::setLimit(const PxJointLinearLimitPair &pair) {
    static_cast<PxPrismaticJoint *>(_nativeJoint)->setLimit(pair);
}

PxJointLinearLimitPair TranslationalJoint::limit() const {
    return static_cast<PxPrismaticJoint *>(_nativeJoint)->getLimit();
}

void TranslationalJoint::setPrismaticJointFlags(PxPrismaticJointFlags flags) {
    static_cast<PxPrismaticJoint *>(_nativeJoint)->setPrismaticJointFlags(flags);
}

void TranslationalJoint::setPrismaticJointFlag(PxPrismaticJointFlag::Enum flag, bool value) {
    static_cast<PxPrismaticJoint *>(_nativeJoint)->setPrismaticJointFlag(flag, value);
}

PxPrismaticJointFlags TranslationalJoint::translationalJointFlags() const {
    return static_cast<PxPrismaticJoint *>(_nativeJoint)->getPrismaticJointFlags();
}

void TranslationalJoint::setProjectionLinearTolerance(float tolerance) {
    static_cast<PxPrismaticJoint *>(_nativeJoint)->setProjectionLinearTolerance(tolerance);
}

float TranslationalJoint::projectionLinearTolerance() const {
    return static_cast<PxPrismaticJoint *>(_nativeJoint)->getProjectionLinearTolerance();
}

void TranslationalJoint::setProjectionAngularTolerance(float tolerance) {
    static_cast<PxPrismaticJoint *>(_nativeJoint)->setProjectionAngularTolerance(tolerance);
}

float TranslationalJoint::projectionAngularTolerance() const {
    return static_cast<PxPrismaticJoint *>(_nativeJoint)->getProjectionAngularTolerance();
}


}
}
