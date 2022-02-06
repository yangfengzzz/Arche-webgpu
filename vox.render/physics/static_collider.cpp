//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "static_collider.h"
#include "physics_manager.h"
#include "../entity.h"

namespace vox {
namespace physics {
StaticCollider::StaticCollider(Entity *entity) :
Collider(entity) {
    const auto &p = entity->transform->worldPosition();
    auto q = entity->transform->worldRotationQuaternion();
    q.normalize();
    
    _nativeActor = PhysicsManager::_nativePhysics()->createRigidStatic(PxTransform(PxVec3(p.x, p.y, p.z),
                                                                                   PxQuat(q.x, q.y, q.z, q.w)));
}

}
}
