//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/physics/physics_manager.h"

#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
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

#ifdef JPH_ENABLE_ASSERTS

// Callback for asserts, connect this to your own assert handler if you have one
static bool AssertFailedImpl(const char *inExpression, const char *inMessage, const char *inFile, uint inLine) {
    // Print to the TTY
    cout << inFile << ":" << inLine << ": (" << inExpression << ") " << (inMessage != nullptr ? inMessage : "") << endl;

    // Breakpoint
    return true;
};

#endif  // JPH_ENABLE_ASSERTS

// Layer that objects can be in, determines which other objects it can collide with
// Typically you at least want to have 1 layer for moving bodies and 1 layer for static bodies, but you can have more
// layers if you want. E.g. you could have a layer for high detail collision (which is not used by the physics
// simulation but only if you do collision testing).
namespace Layers {
constexpr uint8 NON_MOVING = 0;
constexpr uint8 MOVING = 1;
constexpr uint8 NUM_LAYERS = 2;
}  // namespace Layers

// Function that determines if two object layers can collide
bool MyObjectCanCollide(ObjectLayer inObject1, ObjectLayer inObject2) {
    switch (inObject1) {
        case Layers::NON_MOVING:
            return inObject2 == Layers::MOVING;  // Non-moving only collides with moving
        case Layers::MOVING:
            return true;  // Moving collides with everything
        default:
            JPH_ASSERT(false);
            return false;
    }
}

// Each broadphase layer results in a separate bounding volume tree in the broad phase. You at least want to have
// a layer for non-moving and moving objects to avoid having to update a tree full of static objects every frame.
// You can have a 1-on-1 mapping between object layers and broadphase layers (like in this case) but if you have
// many object layers you'll be creating many broad phase trees, which is not efficient. If you want to fine tune
// your broadphase layers define JPH_TRACK_BROADPHASE_STATS and look at the stats reported on the TTY.
namespace BroadPhaseLayers {
constexpr BroadPhaseLayer NON_MOVING(0);
constexpr BroadPhaseLayer MOVING(1);
constexpr uint NUM_LAYERS(2);
}  // namespace BroadPhaseLayers

// BroadPhaseLayerInterface implementation
// This defines a mapping between object and broadphase layers.
class BPLayerInterfaceImpl final : public BroadPhaseLayerInterface {
public:
    BPLayerInterfaceImpl() {
        // Create a mapping table from object to broad phase layer
        mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
        mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
    }

    [[nodiscard]] uint GetNumBroadPhaseLayers() const override { return BroadPhaseLayers::NUM_LAYERS; }

    [[nodiscard]] BroadPhaseLayer GetBroadPhaseLayer(ObjectLayer inLayer) const override {
        JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
        return mObjectToBroadPhase[inLayer];
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    virtual const char *GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override {
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
    BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS]{};
};

// Function that determines if two broadphase layers can collide
bool MyBroadPhaseCanCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) {
    switch (inLayer1) {
        case Layers::NON_MOVING:
            return inLayer2 == BroadPhaseLayers::MOVING;
        case Layers::MOVING:
            return true;
        default:
            JPH_ASSERT(false);
            return false;
    }
}

// An example contact listener
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
    JPH_IF_ENABLE_ASSERTS(AssertFailed = AssertFailedImpl;)

    // Create a factory
    Factory::sInstance = new Factory();

    // Register all Jolt physics types
    RegisterTypes();

    // We need a temp allocator for temporary allocations during the physics update. We're
    // pre-allocating 10 MB to avoid having to do allocations during the physics update.
    // B.t.w. 10 MB is way too much for this example, but it is a typical value you can use.
    // If you don't want to pre-allocate you can also use TempAllocatorMalloc to fall back to
    // malloc / free.
    TempAllocatorImpl temp_allocator(10 * 1024 * 1024);

    // We need a job system that will execute physics jobs on multiple threads. Typically,
    // you would implement the JobSystem interface yourself and let Jolt Physics run on top
    // of your own job scheduler. JobSystemThreadPool is an example implementation.
    JobSystemThreadPool job_system(cMaxPhysicsJobs, cMaxPhysicsBarriers, thread::hardware_concurrency() - 1);

    // This is the max amount of rigid bodies that you can add to the physics system. If you try to add more you'll get
    // an error. Note: This value is low because this is a simple test. For a real project use something in the order of
    // 65536.
    const uint cMaxBodies = 1024;

    // This determines how many mutexes to allocate to protect rigid bodies from concurrent access. Set it to 0 for the
    // default settings.
    const uint cNumBodyMutexes = 0;

    // This is the max amount of body pairs that can be queued at any time (the broad phase will detect overlapping
    // body pairs based on their bounding boxes and will insert them into a queue for the narrowphase). If you make this
    // buffer too small the queue will fill up and the broad phase jobs will start to do narrow phase work. This is
    // slightly less efficient. Note: This value is low because this is a simple test. For a real project use something
    // in the order of 65536.
    const uint cMaxBodyPairs = 1024;

    // This is the maximum size of the contact constraint buffer. If more contacts (collisions between bodies) are
    // detected than this number then these contacts will be ignored and bodies will start interpenetrating / fall
    // through the world. Note: This value is low because this is a simple test. For a real project use something in the
    // order of 10240.
    const uint cMaxContactConstraints = 1024;

    // Create mapping table from object layer to broadphase layer
    // Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
    BPLayerInterfaceImpl broad_phase_layer_interface;

    // Now we can create the actual physics system.
    _physics_system.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints,
                         broad_phase_layer_interface, MyBroadPhaseCanCollide, MyObjectCanCollide);

    _on_contact_enter = [&](const JPH::Body &inBody1, const JPH::Body &inBody2,
                            const JPH::ContactManifold &inManifold) {
        const auto kShape1 = _physical_objects_map[inBody1.GetID().GetIndex()];
        const auto kShape2 = _physical_objects_map[inBody2.GetID().GetIndex()];

        auto scripts = kShape1->entity()->scripts();
        for (const auto &script : scripts) {
            script->onContactEnter(*kShape2, inManifold);
        }

        scripts = kShape2->entity()->scripts();
        for (const auto &script : scripts) {
            script->onContactEnter(*kShape1, inManifold.SwapShapes());
        }
    };

    _on_contact_stay = [&](const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold) {
        const auto kShape1 = _physical_objects_map[inBody1.GetID().GetIndex()];
        const auto kShape2 = _physical_objects_map[inBody2.GetID().GetIndex()];

        auto scripts = kShape1->entity()->scripts();
        for (const auto &script : scripts) {
            script->onContactStay(*kShape2, inManifold);
        }

        scripts = kShape2->entity()->scripts();
        for (const auto &script : scripts) {
            script->onContactStay(*kShape1, inManifold.SwapShapes());
        }
    };

    _on_contact_exit = [&](const JPH::SubShapeIDPair &inSubShapePair) {
        const auto kShape1 = _physical_objects_map[inSubShapePair.GetBody1ID().GetIndex()];
        const auto kShape2 = _physical_objects_map[inSubShapePair.GetBody2ID().GetIndex()];

        auto scripts = kShape1->entity()->scripts();
        for (const auto &script : scripts) {
            script->onContactExit(*kShape2, inSubShapePair.GetSubShapeID2());
        }

        scripts = kShape2->entity()->scripts();
        for (const auto &script : scripts) {
            script->onContactExit(*kShape1, inSubShapePair.GetSubShapeID1());
        }
    };

    _contactListener = std::make_unique<ContactListenerWrapper>(_on_contact_enter, _on_contact_stay, _on_contact_exit);
    _physics_system.SetContactListener(_contactListener.get());
}

PhysicsManager::~PhysicsManager() {
    // Destroy the factory
    delete Factory::sInstance;
    Factory::sInstance = nullptr;

    _contactListener.reset();
}

void PhysicsManager::addCollider(Collider *collider) { _physical_objects_map[collider->getIndex()] = collider; }

void PhysicsManager::removeCollider(Collider *collider) { _physical_objects_map.erase(collider->getIndex()); }

JPH::Body *PhysicsManager::createBody(const JPH::Shape *inShape) {
    JPH::BodyCreationSettings settings(inShape, JPH::Vec3(0.0f, -1.0f, 0.0f), JPH::Quat::sIdentity(),
                                       JPH::EMotionType::Static, Layers::NON_MOVING);
    auto body = _physics_system.GetBodyInterface().CreateBody(settings);
    _physics_system.GetBodyInterface().AddBody(body->GetID(), EActivation::DontActivate);
    return body;
}

}  // namespace vox