//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/physics/physics_manager.h"

#include <Jolt/Core/Factory.h>
#include <Jolt/RegisterTypes.h>

#include <iostream>

#include "vox.render/entity.h"
#include "vox.render/physics/collider.h"
#include "vox.render/script.h"

using namespace JPH;
namespace vox {
namespace {
// Disable common warnings triggered by Jolt, you can use JPH_SUPPRESS_WARNING_PUSH / JPH_SUPPRESS_WARNING_POP to store
// and restore the warning state
JPH_SUPPRESS_WARNINGS
// Callback for traces, connect this to your own trace function if you have one
void TraceImpl(const char *inFMT, ...) {
    // Format the message
    va_list list;
    va_start(list, inFMT);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), inFMT, list);

    // Print to the TTY
    std::cout << buffer << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------
// Function that determines if two object layers can collide
bool MyObjectCanCollide(ObjectLayer inObject1, ObjectLayer inObject2) {
    switch (inObject1) {
        case PhysicsManager::Layers::NON_MOVING:
            return inObject2 == PhysicsManager::Layers::MOVING;  // Non-moving only collides with moving
        case PhysicsManager::Layers::MOVING:
            return true;  // Moving collides with everything
        default:
            JPH_ASSERT(false);
            return false;
    }
}

//----------------------------------------------------------------------------------------------------------------------
// BroadPhaseLayerInterface implementation
// This defines a mapping between object and broadphase layers.
class BPLayerInterfaceImpl final : public BroadPhaseLayerInterface {
public:
    BPLayerInterfaceImpl() {
        // Create a mapping table from object to broad phase layer
        mObjectToBroadPhase[PhysicsManager::Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
        mObjectToBroadPhase[PhysicsManager::Layers::MOVING] = BroadPhaseLayers::MOVING;
    }

    [[nodiscard]] uint GetNumBroadPhaseLayers() const override { return BroadPhaseLayers::NUM_LAYERS; }

    [[nodiscard]] BroadPhaseLayer GetBroadPhaseLayer(ObjectLayer inLayer) const override {
//        JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
        return mObjectToBroadPhase[inLayer];
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    [[nodiscard]] const char *GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override {
        switch ((BroadPhaseLayer::Type)inLayer) {
            case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:
                return "NON_MOVING";
            case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:
                return "MOVING";
            default:
                JPH_ASSERT(false);
                return "INVALID";
        }
    }
#endif  // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
    BroadPhaseLayer mObjectToBroadPhase[PhysicsManager::Layers::NUM_LAYERS]{};
};

// Function that determines if two broadphase layers can collide
bool MyBroadPhaseCanCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) {
    switch (inLayer1) {
        case PhysicsManager::Layers::NON_MOVING:
            return inLayer2 == BroadPhaseLayers::MOVING;
        case PhysicsManager::Layers::MOVING:
            return true;
        default:
            JPH_ASSERT(false);
            return false;
    }
}

//----------------------------------------------------------------------------------------------------------------------
class ContactListenerWrapper : public ContactListener {
public:
    std::function<void(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold)>
            _on_contact_enter;
    std::function<void(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold)>
            _on_contact_stay;
    std::function<void(const JPH::SubShapeIDPair &inSubShapePair)> _on_contact_exit;

    ContactListenerWrapper(std::function<void(const JPH::Body &inBody1,
                                              const JPH::Body &inBody2,
                                              const JPH::ContactManifold &inManifold)> on_contact_enter,
                           std::function<void(const JPH::Body &inBody1,
                                              const JPH::Body &inBody2,
                                              const JPH::ContactManifold &inManifold)> on_contact_stay,
                           std::function<void(const JPH::SubShapeIDPair &inSubShapePair)> on_contact_exit)
        : _on_contact_enter(std::move(on_contact_enter)),
          _on_contact_exit(std::move(on_contact_exit)),
          _on_contact_stay(std::move(on_contact_stay)) {}

    // See: ContactListener
    ValidateResult OnContactValidate(const Body &inBody1,
                                     const Body &inBody2,
                                     const CollideShapeResult &inCollisionResult) override {
        // Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
        return ValidateResult::AcceptAllContactsForThisBodyPair;
    }

    void OnContactAdded(const Body &inBody1,
                        const Body &inBody2,
                        const ContactManifold &inManifold,
                        ContactSettings &ioSettings) override {
        _on_contact_enter(inBody1, inBody2, inManifold);
    }

    void OnContactPersisted(const Body &inBody1,
                            const Body &inBody2,
                            const ContactManifold &inManifold,
                            ContactSettings &ioSettings) override {
        _on_contact_stay(inBody1, inBody2, inManifold);
    }

    void OnContactRemoved(const SubShapeIDPair &inSubShapePair) override { _on_contact_exit(inSubShapePair); }
};

}  // namespace

//----------------------------------------------------------------------------------------------------------------------
PhysicsManager *PhysicsManager::GetSingletonPtr() { return ms_singleton; }

PhysicsManager &PhysicsManager::GetSingleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

PhysicsManager::PhysicsManager() {
    // Register allocation hook
    RegisterDefaultAllocator();

    // Install callbacks
    Trace = TraceImpl;

    // Create a factory
    Factory::sInstance = new Factory();

    // Register all Jolt physics types
    RegisterTypes();

    _temp_allocator = std::make_unique<TempAllocatorImpl>(10 * 1024 * 1024);
    _job_system = std::make_unique<JobSystemThreadPool>(cMaxPhysicsJobs, cMaxPhysicsBarriers,
                                                        thread::hardware_concurrency() - 1);

    // Create mapping table from object layer to broadphase layer
    // Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
    _broadPhaseLayerInterface = std::make_unique<BPLayerInterfaceImpl>();

    // Now we can create the actual physics system.
    _physics_system = std::make_unique<JPH::PhysicsSystem>();
    _physics_system->Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints,
                          *_broadPhaseLayerInterface, MyBroadPhaseCanCollide, MyObjectCanCollide);

    _on_contact_enter = [&](const JPH::Body &inBody1, const JPH::Body &inBody2,
                            const JPH::ContactManifold &inManifold) {
        const auto kShape1 = reinterpret_cast<Collider *>(inBody1.GetUserData());
        const auto kShape2 = reinterpret_cast<Collider *>(inBody2.GetUserData());

        if (kShape1) {
            const auto &scripts = kShape1->entity()->scripts();
            for (const auto &script : scripts) {
                script->onContactEnter(*kShape2, inManifold);
            }
        }

        if (kShape2) {
            const auto &scripts = kShape2->entity()->scripts();
            for (const auto &script : scripts) {
                script->onContactEnter(*kShape1, inManifold.SwapShapes());
            }
        }
    };

    _on_contact_stay = [&](const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold) {
        const auto kShape1 = reinterpret_cast<Collider *>(inBody1.GetUserData());
        const auto kShape2 = reinterpret_cast<Collider *>(inBody2.GetUserData());

        if (kShape1) {
            const auto &scripts = kShape1->entity()->scripts();
            for (const auto &script : scripts) {
                script->onContactStay(*kShape2, inManifold);
            }
        }

        if (kShape2) {
            const auto &scripts = kShape2->entity()->scripts();
            for (const auto &script : scripts) {
                script->onContactStay(*kShape1, inManifold.SwapShapes());
            }
        }
    };

    _on_contact_exit = [&](const JPH::SubShapeIDPair &inSubShapePair) {
        const JPH::BodyInterface &interface = _physics_system->GetBodyInterfaceNoLock();
        const auto kShape1 = reinterpret_cast<Collider *>(interface.GetUserData(inSubShapePair.GetBody1ID()));
        const auto kShape2 = reinterpret_cast<Collider *>(interface.GetUserData(inSubShapePair.GetBody2ID()));

        if (kShape1) {
            const auto &scripts = kShape1->entity()->scripts();
            for (const auto &script : scripts) {
                script->onContactExit(*kShape2, inSubShapePair.GetSubShapeID2());
            }
        }

        if (kShape2) {
            const auto &scripts = kShape2->entity()->scripts();
            for (const auto &script : scripts) {
                script->onContactExit(*kShape1, inSubShapePair.GetSubShapeID1());
            }
        }
    };

    _contactListener = std::make_unique<ContactListenerWrapper>(_on_contact_enter, _on_contact_stay, _on_contact_exit);
    _physics_system->SetContactListener(_contactListener.get());
}

PhysicsManager::~PhysicsManager() {
    // Destroy the factory
    delete Factory::sInstance;
    Factory::sInstance = nullptr;

    _contactListener.reset();
    _temp_allocator.reset();
    _job_system.reset();
    _physics_system.reset();
}

void PhysicsManager::setGravity(const Vector3F &inGravity) {
    _physics_system->SetGravity({inGravity.x, inGravity.y, inGravity.z});
}
Vector3F PhysicsManager::getGravity() const {
    auto gravity = _physics_system->GetGravity();
    return {gravity.GetX(), gravity.GetY(), gravity.GetZ()};
}

void PhysicsManager::setPhysicsSettings(const JPH::PhysicsSettings &inSettings) {
    _physics_system->SetPhysicsSettings(inSettings);
}

const JPH::PhysicsSettings &PhysicsManager::getPhysicsSettings() const { return _physics_system->GetPhysicsSettings(); }

const JPH::BodyInterface &PhysicsManager::getBodyInterface() const { return _physics_system->GetBodyInterface(); }

JPH::BodyInterface &PhysicsManager::getBodyInterface() { return _physics_system->GetBodyInterface(); }

const JPH::BodyInterface &PhysicsManager::getBodyInterfaceNoLock() const {
    return _physics_system->GetBodyInterfaceNoLock();
}

JPH::BodyInterface &PhysicsManager::getBodyInterfaceNoLock() { return _physics_system->GetBodyInterfaceNoLock(); }

const JPH::BodyLockInterfaceNoLock &PhysicsManager::getBodyLockInterfaceNoLock() const {
    return _physics_system->GetBodyLockInterfaceNoLock();
}

const JPH::BodyLockInterfaceLocking &PhysicsManager::getBodyLockInterface() const {
    return _physics_system->GetBodyLockInterface();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsManager::addCollider(Collider *collider) { _colliders.push_back(collider); }

void PhysicsManager::removeCollider(Collider *collider) {
    auto iter = std::find(_colliders.begin(), _colliders.end(), collider);
    if (iter != _colliders.end()) {
        _colliders.erase(iter);
    }
}

void PhysicsManager::update(float delta_time) {
    auto simulate_time = delta_time + rest_time_;
    auto step = static_cast<uint32_t>(std::floor(std::min(max_sum_time_step_, simulate_time) / fixed_time_step_));
    rest_time_ = simulate_time - static_cast<float>(step) * fixed_time_step_;
    for (uint32_t i = 0; i < step; i++) {
        for (auto &script : _on_physics_update_scripts) {
            script->onPhysicsUpdate();
        }
        callColliderOnUpdate();
        _physics_system->Update(fixed_time_step_, collision_steps, integration_sub_steps, _temp_allocator.get(),
                                _job_system.get());
        callColliderOnLateUpdate();
    }
}

void PhysicsManager::callColliderOnUpdate() {
    for (auto &collider : _colliders) {
        collider->onUpdate();
    }
}

void PhysicsManager::callColliderOnLateUpdate() {
    for (auto &collider : _colliders) {
        collider->onLateUpdate();
    }
}

void PhysicsManager::addOnPhysicsUpdateScript(Script *script) { _on_physics_update_scripts.emplace_back(script); }

void PhysicsManager::removeOnPhysicsUpdateScript(Script *script) {
    auto iter = std::find(_on_physics_update_scripts.begin(), _on_physics_update_scripts.end(), script);
    if (iter != _on_physics_update_scripts.end()) {
        _on_physics_update_scripts.erase(iter);
    }
}

//----------------------------------------------------------------------------------------------------------------------
#ifdef JPH_DEBUG_RENDERER
void PhysicsManager::drawBodies(const JPH::BodyManager::DrawSettings &inSettings,
                                JPH::DebugRenderer *inRenderer,
                                const JPH::BodyDrawFilter *inBodyFilter) {
    _physics_system->DrawBodies(inSettings, inRenderer, inBodyFilter);
}

void PhysicsManager::drawConstraints(JPH::DebugRenderer *inRenderer) { _physics_system->DrawConstraints(inRenderer); }

void PhysicsManager::drawConstraintLimits(JPH::DebugRenderer *inRenderer) {
    _physics_system->DrawConstraintLimits(inRenderer);
}

void PhysicsManager::drawConstraintReferenceFrame(JPH::DebugRenderer *inRenderer) {
    _physics_system->DrawConstraintReferenceFrame(inRenderer);
}
#endif  // JPH_DEBUG_RENDERER

//----------------------------------------------------------------------------------------------------------------------
void PhysicsManager::castRay(const JPH::RayCast &inRay,
                             JPH::RayCastBodyCollector &ioCollector,
                             const JPH::BroadPhaseLayerFilter &inBroadPhaseLayerFilter,
                             const JPH::ObjectLayerFilter &inObjectLayerFilter) const {
    _physics_system->GetBroadPhaseQuery().CastRay(inRay, ioCollector, inBroadPhaseLayerFilter, inObjectLayerFilter);
}

void PhysicsManager::collideAABox(const JPH::AABox &inBox,
                                  JPH::CollideShapeBodyCollector &ioCollector,
                                  const JPH::BroadPhaseLayerFilter &inBroadPhaseLayerFilter,
                                  const JPH::ObjectLayerFilter &inObjectLayerFilter) const {
    _physics_system->GetBroadPhaseQuery().CollideAABox(inBox, ioCollector, inBroadPhaseLayerFilter,
                                                       inObjectLayerFilter);
}

void PhysicsManager::collideSphere(const Vector3F &inCenter,
                                   float inRadius,
                                   JPH::CollideShapeBodyCollector &ioCollector,
                                   const JPH::BroadPhaseLayerFilter &inBroadPhaseLayerFilter,
                                   const JPH::ObjectLayerFilter &inObjectLayerFilter) const {
    _physics_system->GetBroadPhaseQuery().CollideSphere({inCenter.x, inCenter.y, inCenter.z}, inRadius, ioCollector,
                                                        inBroadPhaseLayerFilter, inObjectLayerFilter);
}

void PhysicsManager::collidePoint(const Vector3F &inPoint,
                                  JPH::CollideShapeBodyCollector &ioCollector,
                                  const JPH::BroadPhaseLayerFilter &inBroadPhaseLayerFilter,
                                  const JPH::ObjectLayerFilter &inObjectLayerFilter) const {
    _physics_system->GetBroadPhaseQuery().CollidePoint({inPoint.x, inPoint.y, inPoint.z}, ioCollector,
                                                       inBroadPhaseLayerFilter, inObjectLayerFilter);
}

void PhysicsManager::collideOrientedBox(const JPH::OrientedBox &inBox,
                                        JPH::CollideShapeBodyCollector &ioCollector,
                                        const JPH::BroadPhaseLayerFilter &inBroadPhaseLayerFilter,
                                        const JPH::ObjectLayerFilter &inObjectLayerFilter) const {
    _physics_system->GetBroadPhaseQuery().CollideOrientedBox(inBox, ioCollector, inBroadPhaseLayerFilter,
                                                             inObjectLayerFilter);
}

void PhysicsManager::castAABox(const JPH::AABoxCast &inBox,
                               JPH::CastShapeBodyCollector &ioCollector,
                               const JPH::BroadPhaseLayerFilter &inBroadPhaseLayerFilter,
                               const JPH::ObjectLayerFilter &inObjectLayerFilter) const {
    _physics_system->GetBroadPhaseQuery().CastAABox(inBox, ioCollector, inBroadPhaseLayerFilter, inObjectLayerFilter);
}

void PhysicsManager::addConstraint(JPH::Constraint *inConstraint) { _physics_system->AddConstraint(inConstraint); }

void PhysicsManager::removeConstraint(JPH::Constraint *inConstraint) {
    _physics_system->RemoveConstraint(inConstraint);
}

bool PhysicsManager::castRay(const JPH::RayCast &inRay,
                             JPH::RayCastResult &ioHit,
                             const JPH::BroadPhaseLayerFilter &inBroadPhaseLayerFilter,
                             const JPH::ObjectLayerFilter &inObjectLayerFilter,
                             const JPH::BodyFilter &inBodyFilter) const {
    return _physics_system->GetNarrowPhaseQuery().CastRay(inRay, ioHit, inBroadPhaseLayerFilter, inObjectLayerFilter,
                                                          inBodyFilter);
}

void PhysicsManager::castRay(const JPH::RayCast &inRay,
                             const JPH::RayCastSettings &inRayCastSettings,
                             JPH::CastRayCollector &ioCollector,
                             const JPH::BroadPhaseLayerFilter &inBroadPhaseLayerFilter,
                             const JPH::ObjectLayerFilter &inObjectLayerFilter,
                             const JPH::BodyFilter &inBodyFilter,
                             const JPH::ShapeFilter &inShapeFilter) const {
    _physics_system->GetNarrowPhaseQuery().CastRay(inRay, inRayCastSettings, ioCollector, inBroadPhaseLayerFilter,
                                                   inObjectLayerFilter, inBodyFilter, inShapeFilter);
}

void PhysicsManager::collidePoint(const Vector3F &inPoint,
                                  JPH::CollidePointCollector &ioCollector,
                                  const JPH::BroadPhaseLayerFilter &inBroadPhaseLayerFilter,
                                  const JPH::ObjectLayerFilter &inObjectLayerFilter,
                                  const JPH::BodyFilter &inBodyFilter,
                                  const JPH::ShapeFilter &inShapeFilter) const {
    _physics_system->GetNarrowPhaseQuery().CollidePoint({inPoint.x, inPoint.y, inPoint.z}, ioCollector,
                                                        inBroadPhaseLayerFilter, inObjectLayerFilter, inBodyFilter,
                                                        inShapeFilter);
}

void PhysicsManager::collideShape(const JPH::Shape *inShape,
                                  const Vector3F &inShapeScale,
                                  const Matrix4x4F &inCenterOfMassTransform,
                                  const JPH::CollideShapeSettings &inCollideShapeSettings,
                                  JPH::CollideShapeCollector &ioCollector,
                                  const JPH::BroadPhaseLayerFilter &inBroadPhaseLayerFilter,
                                  const JPH::ObjectLayerFilter &inObjectLayerFilter,
                                  const JPH::BodyFilter &inBodyFilter,
                                  const JPH::ShapeFilter &inShapeFilter) const {
    _physics_system->GetNarrowPhaseQuery().CollideShape(
            inShape, {inShapeScale.x, inShapeScale.y, inShapeScale.z},
            {{inCenterOfMassTransform(0, 0), inCenterOfMassTransform(1, 0), inCenterOfMassTransform(2, 0),
              inCenterOfMassTransform(3, 0)},
             {inCenterOfMassTransform(0, 1), inCenterOfMassTransform(1, 1), inCenterOfMassTransform(2, 1),
              inCenterOfMassTransform(3, 1)},
             {inCenterOfMassTransform(0, 2), inCenterOfMassTransform(1, 2), inCenterOfMassTransform(2, 2),
              inCenterOfMassTransform(3, 2)},
             {inCenterOfMassTransform(0, 3), inCenterOfMassTransform(1, 3), inCenterOfMassTransform(2, 3),
              inCenterOfMassTransform(3, 3)}},
            inCollideShapeSettings, ioCollector, inBroadPhaseLayerFilter, inObjectLayerFilter, inBodyFilter,
            inShapeFilter);
}

/// Cast a shape and report any hits to ioCollector
void PhysicsManager::castShape(const JPH::ShapeCast &inShapeCast,
                               const JPH::ShapeCastSettings &inShapeCastSettings,
                               JPH::CastShapeCollector &ioCollector,
                               const JPH::BroadPhaseLayerFilter &inBroadPhaseLayerFilter,
                               const JPH::ObjectLayerFilter &inObjectLayerFilter,
                               const JPH::BodyFilter &inBodyFilter,
                               const JPH::ShapeFilter &inShapeFilter) const {
    _physics_system->GetNarrowPhaseQuery().CastShape(inShapeCast, inShapeCastSettings, ioCollector,
                                                     inBroadPhaseLayerFilter, inObjectLayerFilter, inBodyFilter,
                                                     inShapeFilter);
}

/// Collect all leaf transformed shapes that fall inside world space box inBox
void PhysicsManager::collectTransformedShapes(const JPH::AABox &inBox,
                                              JPH::TransformedShapeCollector &ioCollector,
                                              const JPH::BroadPhaseLayerFilter &inBroadPhaseLayerFilter,
                                              const JPH::ObjectLayerFilter &inObjectLayerFilter,
                                              const JPH::BodyFilter &inBodyFilter,
                                              const JPH::ShapeFilter &inShapeFilter) const {
    _physics_system->GetNarrowPhaseQuery().CollectTransformedShapes(inBox, ioCollector, inBroadPhaseLayerFilter,
                                                                    inObjectLayerFilter, inBodyFilter, inShapeFilter);
}

}  // namespace vox
