//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/physx/physx_collider.h"

#include "vox.render/entity.h"

namespace vox {
using namespace physx;

PhysxCollider::PhysxCollider(Entity *entity) : Component(entity) {
    update_flag_ = entity->transform->registerWorldChangeFlag();
}

void PhysxCollider::OnUpdate() {
    if (update_flag_->flag) {
        const auto &transform = entity()->transform;
        const auto &p = transform->worldPosition();
        auto q = transform->worldRotationQuaternion();
        q.normalize();
        native_actor_->setGlobalPose(PxTransform(PxVec3(p.x, p.y, p.z), PxQuat(q.x, q.y, q.z, q.w)));
        update_flag_->flag = false;

        const auto kWorldScale = transform->lossyWorldScale();
        for (auto &shape : shapes_) {
            shape->SetWorldScale(kWorldScale);
        }
    }
}
}  // namespace vox