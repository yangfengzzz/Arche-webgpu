//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

// You can use Jolt.h in your precompiled header to speed up compilation.
#include <Jolt/Jolt.h>

// Jolt includes
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "vox.render/singleton.h"

namespace vox {
class Collider;
class Script;

class PhysicsManager : public Singleton<PhysicsManager> {
public:
    static PhysicsManager &GetSingleton();

    static PhysicsManager *GetSingletonPtr();

    /** The fixed time step in seconds at which physics are performed. */
    static constexpr float fixed_time_step_ = 1.f / 60;

    /** The max sum of time step in seconds one frame. */
    static constexpr float max_sum_time_step_ = 1.f / 3;

    // If you take larger steps than 1 / 60th of a second you need to do multiple collision steps in order to keep the
    // simulation stable. Do 1 collision step per 1 / 60th of a second (round up).
    static constexpr int collision_steps = 1;

    // If you want more accurate step results you can do multiple sub steps within a collision step. Usually you would
    // set this to 1.
    static constexpr int integration_sub_steps = 1;

    PhysicsManager();

    ~PhysicsManager();

    JPH::Body *createBody(const JPH::Shape *inShape);

    void removeBody(const JPH::BodyID& id);

    /**
     * Add collider into the manager.
     * @param collider - StaticCollider or DynamicCollider.
     */
    void addCollider(Collider *collider);

    /**
     * Remove collider.
     * @param collider - StaticCollider or DynamicCollider.
     */
    void removeCollider(Collider *collider);

public:
    /**
     * Call on every frame to update pose of objects.
     */
    void update(float delta_time);

    void callColliderOnUpdate();

    void callColliderOnLateUpdate();

    void addOnPhysicsUpdateScript(Script *script);

    void removeOnPhysicsUpdateScript(Script *script);

private:
    float rest_time_ = 0;
    std::vector<Script *> _on_physics_update_scripts;

    std::function<void(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold)>
            _on_contact_enter;
    std::function<void(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold)>
            _on_contact_stay;
    std::function<void(const JPH::SubShapeIDPair &inSubShapePair)> _on_contact_exit;
    std::unique_ptr<JPH::ContactListener> _contactListener{nullptr};

    std::unordered_map<uint32_t, Collider *> _physical_objects_map;

    JPH::PhysicsSystem _physics_system;
    // We need a temp allocator for temporary allocations during the physics update. We're
    // pre-allocating 10 MB to avoid having to do allocations during the physics update.
    // B.t.w. 10 MB is way too much for this example, but it is a typical value you can use.
    // If you don't want to pre-allocate you can also use TempAllocatorMalloc to fall back to
    // malloc / free.
    JPH::TempAllocatorImpl _temp_allocator;
    // We need a job system that will execute physics jobs on multiple threads. Typically,
    // you would implement the JobSystem interface yourself and let Jolt Physics run on top
    // of your own job scheduler. JobSystemThreadPool is an example implementation.
    JPH::JobSystemThreadPool _job_system;
};
template <>
inline PhysicsManager *Singleton<PhysicsManager>::ms_singleton{nullptr};
}  // namespace vox