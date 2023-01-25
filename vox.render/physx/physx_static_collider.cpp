//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/physx/physx_static_collider.h"

#include "vox.render/entity.h"
#include "vox.render/physx/physx_manager.h"

namespace vox {
using namespace physx;
PhysxStaticCollider::PhysxStaticCollider(Entity *entity) : PhysxCollider(entity) {
    const auto &p = entity->transform->worldPosition();
    auto q = entity->transform->worldRotationQuaternion();
    q.normalize();

    native_actor_ = PhysxManager::GetSingleton().physics->createRigidStatic(
            PxTransform(PxVec3(p.x, p.y, p.z), PxQuat(q.x, q.y, q.z, q.w)));
}
}  // namespace vox