//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/physx/physx_collider.h"

#include "vox.render/entity.h"
#include "vox.render/physx/physx_manager.h"

namespace vox {
using namespace physx;

std::string PhysxCollider::name() { return "PhysxCollider"; }

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
//        for (auto &shape : shapes_) {
//            shape->SetWorldScale(kWorldScale);
//        }
    }
}

void PhysxCollider::_onEnable() { PhysxManager::GetSingleton().addCollider(this); }

void PhysxCollider::_onDisable() { PhysxManager::GetSingleton().removeCollider(this); }

// MARK: - Reflection
void PhysxCollider::onSerialize(nlohmann::json &data) {}

void PhysxCollider::onDeserialize(nlohmann::json &data) {}

void PhysxCollider::onInspector(ui::WidgetContainer &p_root) {}

}  // namespace vox