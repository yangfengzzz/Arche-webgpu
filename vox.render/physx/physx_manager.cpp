//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/physx/physx_manager.h"

namespace vox {
using namespace physx;

PhysxManager *PhysxManager::GetSingletonPtr() { return ms_singleton; }

PhysxManager &PhysxManager::GetSingleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

PhysxManager::PhysxManager() {
    PxFoundation *g_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, g_allocator_, g_error_callback_);
    physics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_foundation, PxTolerancesScale(), false, nullptr);
}

PhysxManager::~PhysxManager() { physics->release(); }

void PhysxManager::update(float delta_time) {}

}  // namespace vox
