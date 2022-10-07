//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

// You can use Jolt.h in your precompiled header to speed up compilation.
#include <Jolt/Jolt.h>

// Jolt includes
#include <Jolt/Physics/PhysicsSystem.h>

#include "vox.render/singleton.h"

namespace vox {
class Collider;

class PhysicsManager : public Singleton<PhysicsManager> {
public:
    static PhysicsManager &GetSingleton();

    static PhysicsManager *GetSingletonPtr();

    PhysicsManager();

    ~PhysicsManager();

    JPH::Body *createBody(const JPH::Shape *inShape);

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

private:
    std::function<void(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold)>
            _on_contact_enter;
    std::function<void(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold)>
            _on_contact_stay;
    std::function<void(const JPH::SubShapeIDPair &inSubShapePair)> _on_contact_exit;
    std::unique_ptr<JPH::ContactListener> _contactListener{nullptr};

    std::unordered_map<uint32_t, Collider *> _physical_objects_map;

    JPH::PhysicsSystem _physics_system;
};
template <>
inline PhysicsManager *Singleton<PhysicsManager>::ms_singleton{nullptr};
}  // namespace vox