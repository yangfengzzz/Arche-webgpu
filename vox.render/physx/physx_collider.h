//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <PxPhysicsAPI.h>

#include "vox.render/component.h"
#include "vox.render/update_flag.h"

namespace vox {
class PhysxCollider : public Component {
public:
    explicit PhysxCollider(Entity *entity);

    void OnUpdate();

    virtual void OnLateUpdate() {}

    physx::PxRigidActor *native_actor_{};

protected:
    std::unique_ptr<UpdateFlag> update_flag_;
};
}  // namespace vox