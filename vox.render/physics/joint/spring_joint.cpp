//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "spring_joint.h"
#include "../physics_manager.h"
#include "../collider.h"

namespace vox {
namespace physics {
SpringJoint::SpringJoint(Collider *collider0, Collider *collider1) {
    auto actor0 = collider0 ? collider0->handle() : nullptr;
    auto actor1 = collider1 ? collider1->handle() : nullptr;
    _nativeJoint = PxDistanceJointCreate(*PhysicsManager::_nativePhysics(),
                                         actor0, PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)),
                                         actor1, PxTransform(PxVec3(), PxQuat(0, 0, 0, 1)));
}

float SpringJoint::distance() const {
    return static_cast<PxDistanceJoint *>(_nativeJoint)->getDistance();
}

void SpringJoint::setMinDistance(float distance) {
    static_cast<PxDistanceJoint *>(_nativeJoint)->setMinDistance(distance);
}

float SpringJoint::minDistance() const {
    return static_cast<PxDistanceJoint *>(_nativeJoint)->getMinDistance();
}

void SpringJoint::setMaxDistance(float distance) {
    static_cast<PxDistanceJoint *>(_nativeJoint)->setMaxDistance(distance);
}

float SpringJoint::maxDistance() const {
    return static_cast<PxDistanceJoint *>(_nativeJoint)->getMaxDistance();
}

void SpringJoint::setTolerance(float tolerance) {
    static_cast<PxDistanceJoint *>(_nativeJoint)->setTolerance(tolerance);
}

float SpringJoint::tolerance() const {
    return static_cast<PxDistanceJoint *>(_nativeJoint)->getTolerance();
}

void SpringJoint::setStiffness(float stiffness) {
    static_cast<PxDistanceJoint *>(_nativeJoint)->setStiffness(stiffness);
}

float SpringJoint::stiffness() const {
    return static_cast<PxDistanceJoint *>(_nativeJoint)->getStiffness();
}

void SpringJoint::setDamping(float damping) {
    static_cast<PxDistanceJoint *>(_nativeJoint)->setDamping(damping);
}

float SpringJoint::damping() const {
    return static_cast<PxDistanceJoint *>(_nativeJoint)->getDamping();
}

void SpringJoint::setDistanceJointFlags(PxDistanceJointFlags flags) {
    static_cast<PxDistanceJoint *>(_nativeJoint)->setDistanceJointFlags(flags);
}

void SpringJoint::setDistanceJointFlag(PxDistanceJointFlag::Enum flag, bool value) {
    static_cast<PxDistanceJoint *>(_nativeJoint)->setDistanceJointFlag(flag, value);
}

PxDistanceJointFlags SpringJoint::distanceJointFlags() const {
    return static_cast<PxDistanceJoint *>(_nativeJoint)->getDistanceJointFlags();
}

}
}
