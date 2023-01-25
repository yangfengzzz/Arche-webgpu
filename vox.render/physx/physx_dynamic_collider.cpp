//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/physx/physx_dynamic_collider.h"

#include "vox.render/entity.h"
#include "vox.render/physx/physx_manager.h"

namespace vox {
using namespace physx;
PhysxDynamicCollider::PhysxDynamicCollider(Entity *entity) : PhysxCollider(entity) {
    const auto &p = entity->transform->worldPosition();
    auto q = entity->transform->worldRotationQuaternion();
    q.normalize();

    native_actor_ = PhysxManager::GetSingleton().physics->createRigidDynamic(
            PxTransform(PxVec3(p.x, p.y, p.z), PxQuat(q.x, q.y, q.z, q.w)));
}

void PhysxDynamicCollider::OnLateUpdate() {
    const auto &transform = entity()->transform;

    PxTransform pose = native_actor_->getGlobalPose();
    transform->setWorldPosition(Point3F(pose.p.x, pose.p.y, pose.p.z));
    transform->setWorldRotationQuaternion(QuaternionF(pose.q.x, pose.q.y, pose.q.z, pose.q.w));
    update_flag_->flag = false;
}

}  // namespace vox