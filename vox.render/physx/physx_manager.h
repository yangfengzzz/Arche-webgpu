//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <PxPhysicsAPI.h>

#include "vox.render/singleton.h"

namespace vox {
class PhysxManager : public Singleton<PhysxManager> {
public:
    /** The fixed time step in seconds at which physics are performed. */
    static constexpr float fixed_time_step_ = 1.f / 60;

    /** The max sum of time step in seconds one frame. */
    static constexpr float max_sum_time_step_ = 1.f / 3;

    static PhysxManager &GetSingleton();

    static PhysxManager *GetSingletonPtr();

    PhysxManager();

    ~PhysxManager();

    physx::PxPhysics *physics;

    physx::PxScene *physics_manager;

public:
    /**
     * Call on every frame to update pose of objects.
     */
    void update(float delta_time);

private:
    physx::PxDefaultAllocator g_allocator_;
    physx::PxDefaultErrorCallback g_error_callback_;

    float rest_time_ = 0;
};
template <>
inline PhysxManager *Singleton<PhysxManager>::ms_singleton{nullptr};
}  // namespace vox
