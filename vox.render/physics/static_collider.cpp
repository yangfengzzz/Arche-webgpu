//
//  static_collider.cpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/3.
//

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
