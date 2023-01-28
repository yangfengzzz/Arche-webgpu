//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/physx/physx_manager.h"

#include <algorithm>
#include <cmath>

#include "vox.render/physx/physx_collider.h"

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
        for (auto &script : on_physics_update_scripts_) {
            script->onPhysicsUpdate();
        }
        callColliderOnUpdate();
        physics_manager->simulate(fixed_time_step_);
        physics_manager->fetchResults(true);
        callColliderOnLateUpdate();
    }
}

//MARK: - Sync Transform
void PhysxManager::callColliderOnUpdate() {
    for (auto &collider : colliders_) {
        collider->OnUpdate();
    }
}

void PhysxManager::callColliderOnLateUpdate() {
    for (auto &collider : colliders_) {
        collider->OnLateUpdate();
    }
}

void PhysxManager::addCollider(PhysxCollider *collider) {
    colliders_.push_back(collider);
    physics_manager->addActor(*collider->native_actor_);
}

void PhysxManager::removeCollider(PhysxCollider *collider) {
    auto iter = std::find(colliders_.begin(), colliders_.end(), collider);
    if (iter != colliders_.end()) {
        colliders_.erase(iter);
    }

    physics_manager->removeActor(*collider->native_actor_);
}

//MARK:- Update in fixed time
void PhysxManager::addOnPhysicsUpdateScript(Script *script) { on_physics_update_scripts_.emplace_back(script); }

void PhysxManager::removeOnPhysicsUpdateScript(Script *script) {
    auto iter = std::find(on_physics_update_scripts_.begin(), on_physics_update_scripts_.end(), script);
    if (iter != on_physics_update_scripts_.end()) {
        on_physics_update_scripts_.erase(iter);
    }
}


}  // namespace vox
