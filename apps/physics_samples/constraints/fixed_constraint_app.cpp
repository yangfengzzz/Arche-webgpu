//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/constraints/fixed_constraint_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/GroupFilterTable.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Constraints/FixedConstraint.h>

#include "apps/physics_samples/physics_utils.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.toolkit/controls/orbit_control.h"
#include "vox.toolkit/physics_view/physics_view_subpass.h"

namespace vox {
namespace {
class ShowScript : public Script {
public:
    physics_view::PhysicsViewSubpass *_debugger{nullptr};
    JPH::BodyManager::DrawSettings inSettings;

    explicit ShowScript(Entity *entity) : Script(entity) { inSettings.mDrawShape = true; }

    void onPhysicsUpdate() override {
        _debugger->Clear();
        PhysicsManager::GetSingleton().drawBodies(inSettings, _debugger);
        PhysicsManager::GetSingleton().drawConstraints(_debugger);
    }
};

}  // namespace

bool PhysicsFixedConstraintApp::prepare(Platform &platform) {
    ForwardApplication::prepare(platform);

    auto scene = _sceneManager->currentScene();
    auto rootEntity = scene->getRootEntity();
    auto showScript = rootEntity->addComponent<ShowScript>();

    auto debugger = std::make_unique<physics_view::PhysicsViewSubpass>(
            _renderContext.get(), _depthStencilTextureFormat, scene, _mainCamera);
    showScript->_debugger = debugger.get();
    _renderPass->addSubpass(std::move(debugger));

    return true;
}

void PhysicsFixedConstraintApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    scene->ambientLight()->setDiffuseSolidColor(Color(1, 1, 1));
    auto rootEntity = scene->createRootEntity();

    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(30, 30, 30);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    cameraEntity->addComponent<control::OrbitControl>();

    JPH::BodyInterface &body_interface = PhysicsManager::GetSingleton().getBodyInterface();
    {
        PhysicsUtils::createFloor(body_interface);

        float box_size = 4.0f;
        RefConst<Shape> box = new BoxShape(Vec3::sReplicate(0.5f * box_size));

        const int num_bodies = 10;

        // Build a collision group filter that disables collision between adjacent bodies
        Ref<GroupFilterTable> group_filter = new GroupFilterTable(num_bodies);
        for (CollisionGroup::SubGroupID i = 0; i < num_bodies - 1; ++i) group_filter->DisableCollision(i, i + 1);

        // Bodies attached through fixed constraints
        for (int randomness = 0; randomness < 2; ++randomness) {
            CollisionGroup::GroupID group_id = CollisionGroup::GroupID(randomness);

            Vec3 position(0, 25.0f, -randomness * 20.0f);
            Body &top = *body_interface.CreateBody(BodyCreationSettings(
                    box, position, Quat::sIdentity(), EMotionType::Static, PhysicsManager::Layers::NON_MOVING));
            top.SetCollisionGroup(CollisionGroup(group_filter, group_id, 0));
            body_interface.AddBody(top.GetID(), EActivation::DontActivate);

            default_random_engine random;
            uniform_real_distribution<float> displacement(-1.0f, 1.0f);

            Body *prev = &top;
            for (int i = 1; i < num_bodies; ++i) {
                Quat rotation;
                if (randomness == 0) {
                    position += Vec3(box_size, 0, 0);
                    rotation = Quat::sIdentity();
                } else {
                    position += Vec3(box_size + abs(displacement(random)), displacement(random), displacement(random));
                    rotation = Quat::sRandom(random);
                }

                Body &segment = *body_interface.CreateBody(BodyCreationSettings(
                        box, position, rotation, EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
                segment.SetCollisionGroup(CollisionGroup(group_filter, group_id, CollisionGroup::SubGroupID(i)));
                body_interface.AddBody(segment.GetID(), EActivation::Activate);

                FixedConstraintSettings settings;
                settings.mAutoDetectPoint = true;
                Ref<Constraint> c = settings.Create(*prev, segment);
                PhysicsManager::GetSingleton().addConstraint(c);

                prev = &segment;
            }
        }

        {
            // Two light bodies attached to a heavy body (gives issues if the wrong anchor point is chosen)
            Body *light1 = body_interface.CreateBody(
                    BodyCreationSettings(new BoxShape(Vec3::sReplicate(0.1f)), Vec3(-5.0f, 7.0f, -5.2f),
                                         Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
            body_interface.AddBody(light1->GetID(), EActivation::Activate);
            Body *heavy = body_interface.CreateBody(
                    BodyCreationSettings(new BoxShape(Vec3::sReplicate(5.0f)), Vec3(-5.0f, 7.0f, 0.0f),
                                         Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
            body_interface.AddBody(heavy->GetID(), EActivation::Activate);
            Body *light2 = body_interface.CreateBody(
                    BodyCreationSettings(new BoxShape(Vec3::sReplicate(0.1f)), Vec3(-5.0f, 7.0f, 5.2f),
                                         Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
            body_interface.AddBody(light2->GetID(), EActivation::Activate);

            FixedConstraintSettings light1_heavy;
            light1_heavy.mAutoDetectPoint = true;
            PhysicsManager::GetSingleton().addConstraint(light1_heavy.Create(*light1, *heavy));

            FixedConstraintSettings heavy_light2;
            heavy_light2.mAutoDetectPoint = true;
            PhysicsManager::GetSingleton().addConstraint(heavy_light2.Create(*heavy, *light2));
        }

        {
            // A tower of beams and crossbeams (note that it is not recommended to make constructs with this many fixed
            // constraints, this is not always stable)
            Vec3 base_position(0, 25, -40.0f);
            Quat base_rotation = Quat::sRotation(Vec3::sAxisZ(), -0.5f * JPH_PI);

            Ref<BoxShape> pillar = new BoxShape(Vec3(0.1f, 1.0f, 0.1f), 0.0f);
            Ref<BoxShape> beam = new BoxShape(Vec3(0.01f, 1.5f, 0.1f), 0.0f);

            Body *prev_pillars[4] = {&Body::sFixedToWorld, &Body::sFixedToWorld, &Body::sFixedToWorld,
                                     &Body::sFixedToWorld};

            Vec3 center = Vec3::sZero();
            for (int y = 0; y < 10; ++y) {
                // Create pillars
                Body *pillars[4];
                for (int i = 0; i < 4; ++i) {
                    Quat rotation = Quat::sRotation(Vec3::sAxisY(), i * 0.5f * JPH_PI);

                    pillars[i] = body_interface.CreateBody(BodyCreationSettings(
                            pillar, base_position + base_rotation * (center + rotation * Vec3(1.0f, 1.0f, 1.0f)),
                            base_rotation, EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
                    pillars[i]->SetCollisionGroup(CollisionGroup(
                            group_filter, 0,
                            0));  // For convenience, we disable collisions between all objects in the tower
                    body_interface.AddBody(pillars[i]->GetID(), EActivation::Activate);
                }

                for (int i = 0; i < 4; ++i) {
                    Quat rotation = Quat::sRotation(Vec3::sAxisY(), i * 0.5f * JPH_PI);

                    // Create cross beam
                    Body *cross = body_interface.CreateBody(BodyCreationSettings(
                            beam, base_position + base_rotation * (center + rotation * Vec3(1.105f, 1.0f, 0.0f)),
                            base_rotation * rotation * Quat::sRotation(Vec3::sAxisX(), 0.3f * JPH_PI),
                            EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
                    cross->SetCollisionGroup(CollisionGroup(group_filter, 0, 0));
                    body_interface.AddBody(cross->GetID(), EActivation::Activate);

                    // Attach cross beam to pillars
                    for (int j = 0; j < 2; ++j) {
                        FixedConstraintSettings constraint;
                        constraint.mAutoDetectPoint = true;
                        constraint.mNumVelocityStepsOverride =
                                64;  // This structure needs more solver steps to be stable
                        constraint.mNumPositionStepsOverride = JPH_IF_NOT_DEBUG(64) JPH_IF_DEBUG(
                                8);  // In debug mode use less steps to preserve framerate (at the cost of stability)
                        PhysicsManager::GetSingleton().addConstraint(constraint.Create(*pillars[(i + j) % 4], *cross));
                    }

                    // Attach to previous pillar
                    if (prev_pillars[i] != nullptr) {
                        FixedConstraintSettings constraint;
                        constraint.mAutoDetectPoint = true;
                        constraint.mNumVelocityStepsOverride = 64;
                        constraint.mNumPositionStepsOverride = JPH_IF_NOT_DEBUG(64) JPH_IF_DEBUG(8);
                        PhysicsManager::GetSingleton().addConstraint(constraint.Create(*prev_pillars[i], *pillars[i]));
                    }

                    prev_pillars[i] = pillars[i];
                }

                center += Vec3(0.0f, 2.0f, 0.0f);
            }

            // Create top
            Body *top = body_interface.CreateBody(
                    BodyCreationSettings(new BoxShape(Vec3(1.2f, 0.1f, 1.2f)),
                                         base_position + base_rotation * (center + Vec3(0.0f, 0.1f, 0.0f)),
                                         base_rotation, EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
            top->SetCollisionGroup(CollisionGroup(group_filter, 0, 0));
            body_interface.AddBody(top->GetID(), EActivation::Activate);

            // Attach top to pillars
            for (int i = 0; i < 4; ++i) {
                FixedConstraintSettings constraint;
                constraint.mAutoDetectPoint = true;
                PhysicsManager::GetSingleton().addConstraint(constraint.Create(*prev_pillars[i], *top));
            }
        }
    }
    scene->play();
}

}  // namespace vox
