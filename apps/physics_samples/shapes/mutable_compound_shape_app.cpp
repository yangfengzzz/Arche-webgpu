//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/shapes/mutable_compound_shape_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/MutableCompoundShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <Jolt/Physics/Collision/Shape/TaperedCapsuleShape.h>

#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.toolkit/controls/orbit_control.h"
#include "vox.toolkit/physics_view/physics_view_subpass.h"

namespace vox {
namespace {
class ShowScript : public Script {
public:
    int mFrameNumber = 0;
    physics_view::PhysicsViewSubpass* _debugger{nullptr};
    JPH::BodyManager::DrawSettings inSettings;
    JPH::BodyIDVector* mBodyIDs{nullptr};
    JPH::RefConst<JPH::Shape>* mSubCompound{nullptr};

    explicit ShowScript(Entity* entity) : Script(entity) { inSettings.mDrawShape = true; }

    void onPhysicsUpdate() override {
        BodyInterface& no_lock = PhysicsManager::GetSingleton().getBodyInterfaceNoLock();

        uniform_real_distribution<float> roll_distribution(0, 1);

        for (BodyID id : *mBodyIDs) {
            BodyLockWrite lock(PhysicsManager::GetSingleton().getBodyLockInterface(), id);
            if (lock.Succeeded()) {
                Body& body = lock.GetBody();

                // Get the shape
                auto* shape = static_cast<MutableCompoundShape*>(const_cast<Shape*>(body.GetShape()));

                // Remember center of mass from before changes
                Vec3 old_com = shape->GetCenterOfMass();

                // Consistently seeded random engine so that bodies move in a predictable way
                default_random_engine consistent_random;

                // Simulate an engine data structure with strided positions/rotations
                struct PositionRotation {
                    Vec3 mPosition;
                    Quat mRotation;
                };
                Array<PositionRotation> pos_rot;

                // Animate sub shapes
                uint count = shape->GetNumSubShapes();
                for (uint i = 0; i < count; ++i) {
                    const CompoundShape::SubShape& sub_shape = shape->GetSubShape(i);
                    pos_rot.push_back(
                            {Vec3::sZero(), (Quat::sRotation(Vec3::sRandom(consistent_random),
                                                             DegreesToRadians(10.0f) *
                                                                     PhysicsManager::GetSingleton().fixed_time_step_) *
                                             sub_shape.GetRotation())
                                                    .Normalized()});
                }

                // Set the new rotations/orientations on the sub shapes
                shape->ModifyShapes(0, count, &pos_rot.front().mPosition, &pos_rot.front().mRotation,
                                    sizeof(PositionRotation), sizeof(PositionRotation));

                // Initialize frame dependent random number generator
                default_random_engine frame_random(mFrameNumber++);

                // Roll the dice
                float roll = roll_distribution(frame_random);
                if (roll < 0.001f && count > 1) {
                    // Remove a random shape
                    uniform_int_distribution<uint> index_distribution(0, count - 1);
                    shape->RemoveShape(index_distribution(frame_random));
                } else if (roll < 0.002f && count < 10) {
                    // Add a shape in a random rotation
                    shape->AddShape(Vec3::sZero(), Quat::sRandom(frame_random), *mSubCompound);
                }

                // Ensure that the center of mass is updated
                shape->AdjustCenterOfMass();

                // Since we're already locking the body, we don't need to lock it again
                // We always update the mass properties of the shape because we're reorienting them every frame
                no_lock.NotifyShapeChanged(id, old_com, true, EActivation::Activate);
            }
        }

        _debugger->Clear();
        PhysicsManager::GetSingleton().drawBodies(inSettings, _debugger);
    }
};

}  // namespace

bool PhysicsMutableCompoundShapeApp::prepare(Platform& platform) {
    ForwardApplication::prepare(platform);

    auto scene = _sceneManager->currentScene();
    auto rootEntity = scene->getRootEntity();
    auto showScript = rootEntity->addComponent<ShowScript>();

    auto debugger = std::make_unique<physics_view::PhysicsViewSubpass>(
            _renderContext.get(), _depthStencilTextureFormat, scene, _mainCamera);
    showScript->_debugger = debugger.get();
    _renderPass->addSubpass(std::move(debugger));

    showScript->mBodyIDs = &mBodyIDs;
    showScript->mSubCompound = &mSubCompound;

    return true;
}

void PhysicsMutableCompoundShapeApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    scene->ambientLight()->setDiffuseSolidColor(Color(1, 1, 1));
    auto rootEntity = scene->createRootEntity();

    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(30, 30, 30);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    cameraEntity->addComponent<control::OrbitControl>();

    JPH::BodyInterface& body_interface = PhysicsManager::GetSingleton().getBodyInterface();
    {
        // Floor (extra thick because we can randomly add sub shapes that then may stick out underneath the floor and
        // cause objects to be pushed through)
        Body& floor = *body_interface.CreateBody(
                BodyCreationSettings(new BoxShape(Vec3(100.0f, 10.0f, 100.0f), 0.0f), Vec3(0.0f, -10.0f, 0.0f),
                                     Quat::sIdentity(), EMotionType::Static, PhysicsManager::Layers::NON_MOVING));
        body_interface.AddBody(floor.GetID(), EActivation::DontActivate);

        // Compound with sub compound and rotation
        StaticCompoundShapeSettings sub_compound_settings;
        sub_compound_settings.AddShape(Vec3(0, 1.5f, 0), Quat::sRotation(Vec3::sAxisZ(), 0.5f * JPH_PI),
                                       new BoxShape(Vec3(1.5f, 0.25f, 0.2f)));
        sub_compound_settings.AddShape(Vec3(1.5f, 0, 0), Quat::sRotation(Vec3::sAxisZ(), 0.5f * JPH_PI),
                                       new CylinderShape(1.5f, 0.2f));
        sub_compound_settings.AddShape(Vec3(0, 0, 1.5f), Quat::sRotation(Vec3::sAxisX(), 0.5f * JPH_PI),
                                       new TaperedCapsuleShapeSettings(1.5f, 0.25f, 0.2f));
        mSubCompound = sub_compound_settings.Create().Get();

        for (int i = 0; i < 10; ++i) {
            // Create a mutable compound per body and fill it up with 2 shapes initially
            Ref<MutableCompoundShapeSettings> compound_shape = new MutableCompoundShapeSettings;
            compound_shape->AddShape(
                    Vec3::sZero(),
                    Quat::sRotation(Vec3::sAxisX(), -0.25f * JPH_PI) * Quat::sRotation(Vec3::sAxisZ(), 0.25f * JPH_PI),
                    mSubCompound);
            compound_shape->AddShape(
                    Vec3::sZero(),
                    Quat::sRotation(Vec3::sAxisX(), 0.25f * JPH_PI) * Quat::sRotation(Vec3::sAxisZ(), -0.75f * JPH_PI),
                    mSubCompound);

            // Create a body
            Body& body = *body_interface.CreateBody(BodyCreationSettings(compound_shape, Vec3(0, 10.0f + 5.0f * i, 0),
                                                                         Quat::sIdentity(), EMotionType::Dynamic,
                                                                         PhysicsManager::Layers::MOVING));
            body_interface.AddBody(body.GetID(), EActivation::Activate);
            mBodyIDs.push_back(body.GetID());
        }
    }
    scene->play();
}

}  // namespace vox
