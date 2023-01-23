//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/physx/physx_manager.h"
#include <algorithm>
#include <cmath>
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
    
    PxSceneDesc scene_desc(physics->getTolerancesScale());
    scene_desc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    scene_desc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
    scene_desc.filterShader = PxDefaultSimulationFilterShader;
    scene_desc.kineKineFilteringMode = PxPairFilteringMode::eKEEP;
    scene_desc.staticKineFilteringMode = PxPairFilteringMode::eKEEP;
    scene_desc.flags |= PxSceneFlag::eENABLE_CCD;

    physics_manager = physics->createScene(scene_desc);
}

PhysxManager::~PhysxManager() { physics->release(); }

void PhysxManager::update(float delta_time) {
    float simulate_time = delta_time + rest_time_;
    auto step = static_cast<uint32_t>(std::floor(std::min(max_sum_time_step_, simulate_time) / fixed_time_step_));
    rest_time_ = simulate_time - static_cast<float>(step) * fixed_time_step_;
    for (uint32_t i = 0; i < step; i++) {
        physics_manager->simulate(fixed_time_step_);
        physics_manager->fetchResults(true);
    }
}

}  // namespace vox
