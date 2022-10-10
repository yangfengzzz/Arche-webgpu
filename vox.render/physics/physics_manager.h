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

#include "vox.math/vector3.h"
#include "vox.render/singleton.h"

namespace vox {
class Collider;
class Script;

// Each broadphase layer results in a separate bounding volume tree in the broad phase. You at least want to have
// a layer for non-moving and moving objects to avoid having to update a tree full of static objects every frame.
// You can have a 1-on-1 mapping between object layers and broadphase layers (like in this case) but if you have
// many object layers you'll be creating many broad phase trees, which is not efficient. If you want to fine tune
// your broadphase layers define JPH_TRACK_BROADPHASE_STATS and look at the stats reported on the TTY.
namespace BroadPhaseLayers {
constexpr JPH::BroadPhaseLayer NON_MOVING(0);
constexpr JPH::BroadPhaseLayer MOVING(1);
constexpr uint NUM_LAYERS(2);
}  // namespace BroadPhaseLayers

class PhysicsManager : public Singleton<PhysicsManager> {
public:
    static PhysicsManager &GetSingleton();

    static PhysicsManager *GetSingletonPtr();

    // Layer that objects can be in, determines which other objects it can collide with
    // Typically you at least want to have 1 layer for moving bodies and 1 layer for static bodies, but you can have
    // more layers if you want. E.g. you could have a layer for high detail collision (which is not used by the physics
    // simulation but only if you do collision testing).
    enum Layers : uint8_t { NON_MOVING = 0, MOVING = 1, NUM_LAYERS = 2 };

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

    // This is the max amount of rigid bodies that you can add to the physics system. If you try to add more you'll get
    // an error. Note: This value is low because this is a simple test. For a real project use something in the order of
    // 65536.
    static constexpr uint cMaxBodies = 1024;

    // This determines how many mutexes to allocate to protect rigid bodies from concurrent access. Set it to 0 for the
    // default settings.
    static constexpr uint cNumBodyMutexes = 0;

    // This is the max amount of body pairs that can be queued at any time (the broad phase will detect overlapping
    // body pairs based on their bounding boxes and will insert them into a queue for the narrowphase). If you make this
    // buffer too small the queue will fill up and the broad phase jobs will start to do narrow phase work. This is
    // slightly less efficient. Note: This value is low because this is a simple test. For a real project use something
    // in the order of 65536.
    static constexpr uint cMaxBodyPairs = 1024;

    // This is the maximum size of the contact constraint buffer. If more contacts (collisions between bodies) are
    // detected than this number then these contacts will be ignored and bodies will start interpenetrating / fall
    // through the world. Note: This value is low because this is a simple test. For a real project use something in the
    // order of 10240.
    static constexpr uint cMaxContactConstraints = 1024;

    /// Set gravity value
    void setGravity(const Vector3F &inGravity);

    [[nodiscard]] Vector3F getGravity() const;

    /// Control the main constants of the physics simulation
    void setPhysicsSettings(const JPH::PhysicsSettings &inSettings);

    [[nodiscard]] const JPH::PhysicsSettings &getPhysicsSettings() const;

    PhysicsManager();

    ~PhysicsManager();

    [[nodiscard]] const JPH::BodyInterface &getBodyInterface() const;

    [[nodiscard]] JPH::BodyInterface &getBodyInterface();

    [[nodiscard]] const JPH::BodyInterface &getBodyInterfaceNoLock() const;

    [[nodiscard]] JPH::BodyInterface &getBodyInterfaceNoLock();

    [[nodiscard]] const JPH::BodyLockInterfaceNoLock &getBodyLockInterfaceNoLock() const;

    [[nodiscard]] const JPH::BodyLockInterfaceLocking &getBodyLockInterface() const;

#ifdef JPH_DEBUG_RENDERER
    /// Draw the state of the bodies (debugging purposes)
    void drawBodies(const JPH::BodyManager::DrawSettings &inSettings,
                    JPH::DebugRenderer *inRenderer,
                    const JPH::BodyDrawFilter *inBodyFilter = nullptr);

    /// Draw the constraints only (debugging purposes)
    void drawConstraints(JPH::DebugRenderer *inRenderer);

    /// Draw the constraint limits only (debugging purposes)
    void drawConstraintLimits(JPH::DebugRenderer *inRenderer);

    /// Draw the constraint reference frames only (debugging purposes)
    void drawConstraintReferenceFrame(JPH::DebugRenderer *inRenderer);
#endif  // JPH_DEBUG_RENDERER

public:
    /// Cast a ray and add any hits to ioCollector
    void castRay(const JPH::RayCast &inRay,
                 JPH::RayCastBodyCollector &ioCollector,
                 const JPH::BroadPhaseLayerFilter &inBroadPhaseLayerFilter = {},
                 const JPH::ObjectLayerFilter &inObjectLayerFilter = {}) const;

    /// Get bodies intersecting with inBox and any hits to ioCollector
    void collideAABox(const JPH::AABox &inBox,
                      JPH::CollideShapeBodyCollector &ioCollector,
                      const JPH::BroadPhaseLayerFilter &inBroadPhaseLayerFilter = {},
                      const JPH::ObjectLayerFilter &inObjectLayerFilter = {}) const;

    /// Get bodies intersecting with a sphere and any hits to ioCollector
    void collideSphere(const Vector3F &inCenter,
                       float inRadius,
                       JPH::CollideShapeBodyCollector &ioCollector,
                       const JPH::BroadPhaseLayerFilter &inBroadPhaseLayerFilter = {},
                       const JPH::ObjectLayerFilter &inObjectLayerFilter = {}) const;

    /// Get bodies intersecting with a point and any hits to ioCollector
    void collidePoint(const Vector3F &inPoint,
                      JPH::CollideShapeBodyCollector &ioCollector,
                      const JPH::BroadPhaseLayerFilter &inBroadPhaseLayerFilter = {},
                      const JPH::ObjectLayerFilter &inObjectLayerFilter = {}) const;

    /// Get bodies intersecting with an oriented box and any hits to ioCollector
    void collideOrientedBox(const JPH::OrientedBox &inBox,
                            JPH::CollideShapeBodyCollector &ioCollector,
                            const JPH::BroadPhaseLayerFilter &inBroadPhaseLayerFilter = {},
                            const JPH::ObjectLayerFilter &inObjectLayerFilter = {}) const;

    /// Cast a box and add any hits to ioCollector
    void castAABox(const JPH::AABoxCast &inBox,
                   JPH::CastShapeBodyCollector &ioCollector,
                   const JPH::BroadPhaseLayerFilter &inBroadPhaseLayerFilter = {},
                   const JPH::ObjectLayerFilter &inObjectLayerFilter = {}) const;

public:
    /// Add constraint to the world
    void addConstraint(JPH::Constraint *inConstraint);

    /// Remove constraint from the world
    void removeConstraint(JPH::Constraint *inConstraint);

public:
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
    std::vector<Script *> _on_physics_update_scripts{};
    std::vector<Collider *> _colliders{};

    std::function<void(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold)>
            _on_contact_enter{};
    std::function<void(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold)>
            _on_contact_stay{};
    std::function<void(const JPH::SubShapeIDPair &inSubShapePair)> _on_contact_exit{};
    std::unique_ptr<JPH::ContactListener> _contactListener{nullptr};
    std::unique_ptr<JPH::BroadPhaseLayerInterface> _broadPhaseLayerInterface{nullptr};

    std::unique_ptr<JPH::PhysicsSystem> _physics_system{nullptr};
    // We need a temp allocator for temporary allocations during the physics update. We're
    // pre-allocating 10 MB to avoid having to do allocations during the physics update.
    // B.t.w. 10 MB is way too much for this example, but it is a typical value you can use.
    // If you don't want to pre-allocate you can also use TempAllocatorMalloc to fall back to
    // malloc / free.
    std::unique_ptr<JPH::TempAllocatorImpl> _temp_allocator{nullptr};
    // We need a job system that will execute physics jobs on multiple threads. Typically,
    // you would implement the JobSystem interface yourself and let Jolt Physics run on top
    // of your own job scheduler. JobSystemThreadPool is an example implementation.
    std::unique_ptr<JPH::JobSystemThreadPool> _job_system{nullptr};
};
template <>
inline PhysicsManager *Singleton<PhysicsManager>::ms_singleton{nullptr};
}  // namespace vox