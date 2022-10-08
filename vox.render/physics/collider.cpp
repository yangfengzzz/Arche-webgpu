//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/physics/collider.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>

#include "vox.render/physics/physics_manager.h"

namespace vox {
Collider::Collider(Entity* entity) : Component(entity) {}

Collider::~Collider() {
    if (!_bodyID.IsInvalid()) {
        _bodyInterface->RemoveBody(_bodyID);
        _bodyInterface->DestroyBody(_bodyID);
    }
}

void Collider::setShape(const std::shared_ptr<JPH::Shape>& shape) {
    _shape = shape;
    if (!_bodyID.IsInvalid()) {
        _bodyInterface->RemoveBody(_bodyID);
        _bodyInterface->DestroyBody(_bodyID);
    }

    JPH::BodyCreationSettings settings(shape.get(), JPH::Vec3(0.0f, -1.0f, 0.0f), JPH::Quat::sIdentity(),
                                       JPH::EMotionType::Static, PhysicsManager::Layers::NON_MOVING);
    auto body = _bodyInterface->CreateBody(settings);
    body->SetUserData(reinterpret_cast<uint64_t>(this));
    _bodyID = body->GetID();
    _bodyInterface->AddBody(_bodyID, JPH::EActivation::DontActivate);
}

std::shared_ptr<JPH::Shape> Collider::getShape() { return _shape; }

void Collider::_onEnable() {
    PhysicsManager::GetSingleton().addCollider(this);
    _bodyInterface = &PhysicsManager::GetSingleton().getBodyInterface();
}

void Collider::_onDisable() { PhysicsManager::GetSingleton().removeCollider(this); }

}  // namespace vox