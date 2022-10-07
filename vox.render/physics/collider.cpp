//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/physics/collider.h"

#include "vox.render/physics/physics_manager.h"

namespace vox {
Collider::Collider(Entity *entity) : Component(entity) {}

uint32_t Collider::getIndex() const { return _body->GetID().GetIndex(); }

void Collider::setShape(const JPH::Shape *inShape) { _body = PhysicsManager::GetSingleton().createBody(inShape); }

void Collider::_onEnable() { PhysicsManager::GetSingleton().addCollider(this); }

void Collider::_onDisable() { PhysicsManager::GetSingleton().removeCollider(this); }

}  // namespace vox