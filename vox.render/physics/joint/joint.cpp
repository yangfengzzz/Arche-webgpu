//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "joint.h"
#include "../collider.h"

namespace vox {
namespace physics {
void Joint::setActors(Collider *actor0, Collider *actor1) {
    _nativeJoint->setActors(actor0->handle(), actor1->handle());
}

void Joint::setLocalPose(PxJointActorIndex::Enum actor, const Transform3F &localPose) {
    const auto &p = localPose.translation();
    const auto &q = localPose.orientation();
    _nativeJoint->setLocalPose(actor, PxTransform(PxVec3(p.x, p.y, p.z), PxQuat(q.x, q.y, q.z, q.w)));
}

Transform3F Joint::localPose(PxJointActorIndex::Enum actor) const {
    const auto pose = _nativeJoint->getLocalPose(actor);
    Transform3F trans;
    trans.setTranslation(Vector3F(pose.p.x, pose.p.y, pose.p.z));
    trans.setOrientation(QuaternionF(pose.q.x, pose.q.y, pose.q.z, pose.q.w));
    return trans;
}

Transform3F Joint::relativeTransform() const {
    const auto pose = _nativeJoint->getRelativeTransform();
    Transform3F trans;
    trans.setTranslation(Vector3F(pose.p.x, pose.p.y, pose.p.z));
    trans.setOrientation(QuaternionF(pose.q.x, pose.q.y, pose.q.z, pose.q.w));
    return trans;
}

Vector3F Joint::relativeLinearVelocity() const {
    const auto vel = _nativeJoint->getRelativeLinearVelocity();
    return Vector3F(vel.x, vel.y, vel.z);
}

Vector3F Joint::relativeAngularVelocity() const {
    const auto vel = _nativeJoint->getRelativeAngularVelocity();
    return Vector3F(vel.x, vel.y, vel.z);
}

void Joint::setBreakForce(float force, float torque) {
    _nativeJoint->setBreakForce(force, torque);
}

void Joint::getBreakForce(float &force, float &torque) const {
    _nativeJoint->getBreakForce(force, torque);
}

void Joint::setConstraintFlags(PxConstraintFlags flags) {
    _nativeJoint->setConstraintFlags(flags);
}

void Joint::setConstraintFlag(PxConstraintFlag::Enum flag, bool value) {
    _nativeJoint->setConstraintFlag(flag, value);
}

PxConstraintFlags Joint::constraintFlags() const {
    return _nativeJoint->getConstraintFlags();
}

void Joint::setInvMassScale0(float invMassScale) {
    _nativeJoint->setInvMassScale0(invMassScale);
}

float Joint::invMassScale0() const {
    return _nativeJoint->getInvMassScale0();
}

void Joint::setInvInertiaScale0(float invInertiaScale) {
    _nativeJoint->setInvInertiaScale0(invInertiaScale);
}

float Joint::invInertiaScale0() const {
    return _nativeJoint->getInvInertiaScale0();
}

void Joint::setInvMassScale1(float invMassScale) {
    _nativeJoint->setInvMassScale1(invMassScale);
}

float Joint::invMassScale1() const {
    return _nativeJoint->getInvMassScale1();
}

void Joint::setInvInertiaScale1(float invInertiaScale) {
    _nativeJoint->setInvInertiaScale1(invInertiaScale);
}

float Joint::invInertiaScale1() const {
    return _nativeJoint->getInvInertiaScale1();
}

}
}
