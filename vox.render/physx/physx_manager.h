//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/singleton.h"
#include <PxPhysicsAPI.h>

namespace vox {
class PhysxManager : public Singleton<PhysxManager> {
public:
    static PhysxManager &GetSingleton();

    static PhysxManager *GetSingletonPtr();

    PhysxManager();

    ~PhysxManager();

    physx::PxPhysics *physics;

public:
    /**
     * Call on every frame to update pose of objects.
     */
    void update(float delta_time);

private:
    physx::PxDefaultAllocator g_allocator_;
    physx::PxDefaultErrorCallback g_error_callback_;
};
template <>
inline PhysxManager *Singleton<PhysxManager>::ms_singleton{nullptr};
}  // namespace vox