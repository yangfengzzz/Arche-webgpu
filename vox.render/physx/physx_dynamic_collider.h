//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/physx/physx_collider.h"

namespace vox {
class PhysxDynamicCollider: public PhysxCollider {
public:
    explicit PhysxDynamicCollider(Entity *entity);

private:
    void OnLateUpdate() override;
};
}