//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/physics/collider.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>

#include "vox.render/entity.h"
#include "vox.render/physics/physics_manager.h"

namespace vox {
std::string Collider::name() { return "Collider"; }

Collider::Collider(Entity* entity) : Component(entity) { update_flag_ = entity->transform->registerWorldChangeFlag(); }

Collider::~Collider() {
    if (!_bodyID.IsInvalid()) {
        _bodyInterface->RemoveBody(_bodyID);
        _bodyInterface->DestroyBody(_bodyID);
    }
}

void Collider::setShape(std::unique_ptr<JPH::Shape>&& shape, JPH::EMotionType type) {
    if (!_bodyID.IsInvalid()) {
        _bodyInterface->SetShape(_bodyID, shape.get(), true, JPH::EActivation::Activate);
        _bodyInterface->SetMotionType(_bodyID, type, JPH::EActivation::Activate);
        _shape.reset();
    } else {
        JPH::BodyCreationSettings settings(
                shape.get(), JPH::Vec3(), JPH::Quat::sIdentity(), type,
                type == JPH::EMotionType::Static ? PhysicsManager::Layers::NON_MOVING : PhysicsManager::Layers::MOVING);
        _body = _bodyInterface->CreateBody(settings);
        _body->SetUserData(reinterpret_cast<uint64_t>(this));
        _bodyID = _body->GetID();
        _bodyInterface->AddBody(_bodyID, JPH::EActivation::DontActivate);
    }
    update_flag_->flag = true;
    _shape = std::move(shape);
}

const JPH::Shape& Collider::getShape() { return *_shape; }

Vector3F Collider::getCenterOfMassPosition() const {
    auto com = _bodyInterface->GetCenterOfMassPosition(_bodyID);
    return {com.GetX(), com.GetY(), com.GetZ()};
}

Matrix4x4F Collider::getCenterOfMassTransform() const {
    auto com = _bodyInterface->GetCenterOfMassTransform(_bodyID);
    return {com(0, 0), com(1, 0), com(2, 0), com(3, 0),  //
            com(0, 1), com(1, 1), com(2, 1), com(3, 1),  //
            com(0, 2), com(1, 2), com(2, 2), com(3, 2),  //
            com(0, 3), com(1, 3), com(2, 3), com(3, 3)};
}

void Collider::setLinearAndAngularVelocity(const Vector3F& inLinearVelocity, const Vector3F& inAngularVelocity) {
    _bodyInterface->SetLinearAndAngularVelocity(_bodyID, {inLinearVelocity.x, inLinearVelocity.y, inLinearVelocity.z},
                                                {inAngularVelocity.x, inAngularVelocity.y, inAngularVelocity.z});
}

void Collider::getLinearAndAngularVelocity(Vector3F& outLinearVelocity, Vector3F& outAngularVelocity) const {
    JPH::Vec3 oVel{}, oAngVel{};
    _bodyInterface->GetLinearAndAngularVelocity(_bodyID, oVel, oAngVel);
    outLinearVelocity = {oVel.GetX(), oVel.GetY(), oVel.GetZ()};
    outAngularVelocity = {oAngVel.GetX(), oAngVel.GetY(), oAngVel.GetZ()};
}

void Collider::setLinearVelocity(const Vector3F& inLinearVelocity) {
    _bodyInterface->SetLinearVelocity(_bodyID, {inLinearVelocity.x, inLinearVelocity.y, inLinearVelocity.z});
}

Vector3F Collider::getLinearVelocity() const {
    auto vel = _bodyInterface->GetLinearVelocity(_bodyID);
    return {vel.GetX(), vel.GetY(), vel.GetZ()};
}

void Collider::addLinearVelocity(const Vector3F& inLinearVelocity) {
    _bodyInterface->AddLinearVelocity(_bodyID, {inLinearVelocity.x, inLinearVelocity.y, inLinearVelocity.z});
}

void Collider::addLinearAndAngularVelocity(const Vector3F& inLinearVelocity, const Vector3F& inAngularVelocity) {
    _bodyInterface->AddLinearAndAngularVelocity(_bodyID, {inLinearVelocity.x, inLinearVelocity.y, inLinearVelocity.z},
                                                {inAngularVelocity.x, inAngularVelocity.y, inAngularVelocity.z});
}

void Collider::setAngularVelocity(const Vector3F& inAngularVelocity) {
    _bodyInterface->SetAngularVelocity(_bodyID, {inAngularVelocity.x, inAngularVelocity.y, inAngularVelocity.z});
}

Vector3F Collider::getAngularVelocity() const {
    auto vel = _bodyInterface->GetAngularVelocity(_bodyID);
    return {vel.GetX(), vel.GetY(), vel.GetZ()};
}

Vector3F Collider::getPointVelocity(const Vector3F& inPoint) const {
    auto vel = _bodyInterface->GetPointVelocity(_bodyID, {inPoint.x, inPoint.y, inPoint.z});
    return {vel.GetX(), vel.GetY(), vel.GetZ()};
}

void Collider::addForce(const Vector3F& inForce) {
    _bodyInterface->AddForce(_bodyID, {inForce.x, inForce.y, inForce.z});
}

void Collider::addForce(const Vector3F& inForce, const Vector3F& inPoint) {
    _bodyInterface->AddForce(_bodyID, {inForce.x, inForce.y, inForce.z}, {inPoint.x, inPoint.y, inPoint.z});
}

void Collider::addTorque(const Vector3F& inTorque) {
    _bodyInterface->AddTorque(_bodyID, {inTorque.x, inTorque.y, inTorque.z});
}

void Collider::addForceAndTorque(const Vector3F& inForce, const Vector3F& inTorque) {
    _bodyInterface->AddForceAndTorque(_bodyID, {inForce.x, inForce.y, inForce.z}, {inTorque.x, inTorque.y, inTorque.z});
}

void Collider::addImpulse(const Vector3F& inImpulse) {
    _bodyInterface->AddImpulse(_bodyID, {inImpulse.x, inImpulse.y, inImpulse.z});
}

void Collider::addImpulse(const Vector3F& inImpulse, const Vector3F& inPoint) {
    _bodyInterface->AddImpulse(_bodyID, {inImpulse.x, inImpulse.y, inImpulse.z}, {inPoint.x, inPoint.y, inPoint.z});
}

void Collider::addAngularImpulse(const Vector3F& inAngularImpulse) {
    _bodyInterface->AddAngularImpulse(_bodyID, {inAngularImpulse.x, inAngularImpulse.y, inAngularImpulse.z});
}

void Collider::setMotionType(JPH::EMotionType inMotionType, JPH::EActivation inActivationMode) {
    _bodyInterface->SetMotionType(_bodyID, inMotionType, inActivationMode);
}

JPH::EMotionType Collider::getMotionType() const { return _bodyInterface->GetMotionType(_bodyID); }

Matrix4x4F Collider::getInverseInertia() const {
    auto inertia = _bodyInterface->GetInverseInertia(_bodyID);
    return {inertia(0, 0), inertia(1, 0), inertia(2, 0), inertia(3, 0),  //
            inertia(0, 1), inertia(1, 1), inertia(2, 1), inertia(3, 1),  //
            inertia(0, 2), inertia(1, 2), inertia(2, 2), inertia(3, 2),  //
            inertia(0, 3), inertia(1, 3), inertia(2, 3), inertia(3, 3)};
}

void Collider::setRestitution(float inRestitution) { _bodyInterface->SetRestitution(_bodyID, inRestitution); }

float Collider::getRestitution() const { return _bodyInterface->GetRestitution(_bodyID); }

void Collider::setFriction(float inFriction) { _bodyInterface->SetFriction(_bodyID, inFriction); }

float Collider::getFriction() const { return _bodyInterface->GetFriction(_bodyID); }

void Collider::setGravityFactor(float inGravityFactor) { _bodyInterface->SetGravityFactor(_bodyID, inGravityFactor); }

float Collider::getGravityFactor() const { return _bodyInterface->GetGravityFactor(_bodyID); }

void Collider::_onEnable() {
    PhysicsManager::GetSingleton().addCollider(this);
    _bodyInterface = &PhysicsManager::GetSingleton().getBodyInterface();
}

void Collider::_onDisable() { PhysicsManager::GetSingleton().removeCollider(this); }

void Collider::onSerialize(nlohmann::json &data) {}

void Collider::onDeserialize(nlohmann::json &data) {}

void Collider::onInspector(ui::WidgetContainer &p_root) {}

void Collider::onUpdate() {
    if (update_flag_->flag) {
        const auto& transform = entity()->transform;
        const auto& p = transform->worldPosition();
        auto q = transform->worldRotationQuaternion();
        q.normalize();
        const auto kWorldScale = transform->lossyWorldScale();
        update_flag_->flag = false;

        _bodyInterface->SetPositionAndRotation(_bodyID, {p.x, p.y, p.z}, {q.x, q.y, q.z, q.w},
                                               JPH::EActivation::Activate);
        _shape->ScaleShape({kWorldScale.x, kWorldScale.y, kWorldScale.z});
    }
};

void Collider::onLateUpdate() {
    if (!_body->IsStatic()) {
        const auto& transform = entity()->transform;
        auto pose = _bodyInterface->GetWorldTransform(_bodyID);
        transform->setWorldMatrix({pose(0, 0), pose(1, 0), pose(2, 0), pose(3, 0),  //
                                   pose(0, 1), pose(1, 1), pose(2, 1), pose(3, 1),  //
                                   pose(0, 2), pose(1, 2), pose(2, 2), pose(3, 2),  //
                                   pose(0, 3), pose(1, 3), pose(2, 3), pose(3, 3)});
        update_flag_->flag = false;
    }
}

}  // namespace vox