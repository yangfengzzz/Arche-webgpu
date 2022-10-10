//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/constraints/slider_constraint_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/GroupFilterTable.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Constraints/SliderConstraint.h>

#include "apps/physics_samples/physics_utils.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.toolkit/controls/orbit_control.h"
#include "vox.toolkit/physics_debugger/physics_debug_subpass.h"

namespace vox {
namespace {
class ShowScript : public Script {
public:
    physics_debugger::PhysicsDebugSubpass *_debugger{nullptr};
    JPH::BodyManager::DrawSettings inSettings;

    explicit ShowScript(Entity *entity) : Script(entity) { inSettings.mDrawShape = true; }

    void onPhysicsUpdate() override {
        _debugger->Clear();
        PhysicsManager::GetSingleton().drawBodies(inSettings, _debugger);
        PhysicsManager::GetSingleton().drawConstraints(_debugger);
    }
};

}  // namespace

bool PhysicsSliderConstraintApp::prepare(Platform &platform) {
    ForwardApplication::prepare(platform);

    auto scene = _sceneManager->currentScene();
    auto rootEntity = scene->getRootEntity();
    auto showScript = rootEntity->addComponent<ShowScript>();

    auto debugger = std::make_unique<physics_debugger::PhysicsDebugSubpass>(
            _renderContext.get(), _depthStencilTextureFormat, scene, _mainCamera);
    showScript->_debugger = debugger.get();
    _renderPass->addSubpass(std::move(debugger));

    return true;
}

void PhysicsSliderConstraintApp::loadScene() {
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

        const int cChainLength = 10;

        // Create group filter
        Ref<GroupFilterTable> group_filter = new GroupFilterTable(cChainLength);
        for (CollisionGroup::SubGroupID i = 0; i < cChainLength - 1; ++i) group_filter->DisableCollision(i, i + 1);
        CollisionGroup::GroupID group_id = 0;

        // Create box
        float box_size = 4.0f;
        RefConst<Shape> box = new BoxShape(Vec3::sReplicate(0.5f * box_size));

        // Bodies attached through slider constraints
        for (int randomness = 0; randomness < 2; ++randomness) {
            Vec3 position(0, 25.0f, -randomness * 20.0f);
            Body &top = *body_interface.CreateBody(BodyCreationSettings(
                    box, position, Quat::sIdentity(), EMotionType::Static, PhysicsManager::Layers::NON_MOVING));
            top.SetCollisionGroup(CollisionGroup(group_filter, group_id, 0));
            body_interface.AddBody(top.GetID(), EActivation::DontActivate);

            default_random_engine random;
            uniform_real_distribution<float> displacement(-1.0f, 1.0f);

            Body *prev = &top;
            for (int i = 1; i < cChainLength; ++i) {
                Quat rotation;
                Vec3 slider_axis;
                if (randomness == 0) {
                    position += Vec3(box_size, 0, 0);
                    rotation = Quat::sIdentity();
                    slider_axis = Quat::sRotation(Vec3::sAxisZ(), -DegreesToRadians(10)).RotateAxisX();
                } else {
                    position += Vec3(box_size + abs(displacement(random)), displacement(random), displacement(random));
                    rotation = Quat::sRandom(random);
                    slider_axis = Quat::sRotation(Vec3::sAxisY(), displacement(random) * DegreesToRadians(20)) *
                                  Quat::sRotation(Vec3::sAxisZ(), -DegreesToRadians(10)).RotateAxisX();
                }

                Body &segment = *body_interface.CreateBody(BodyCreationSettings(
                        box, position, rotation, EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
                segment.SetCollisionGroup(CollisionGroup(group_filter, group_id, CollisionGroup::SubGroupID(i)));
                body_interface.AddBody(segment.GetID(), EActivation::Activate);

                SliderConstraintSettings settings;
                settings.mAutoDetectPoint = true;
                settings.SetSliderAxis(slider_axis);
                settings.mLimitsMin = -5.0f;
                settings.mLimitsMax = 10.0f;
                PhysicsManager::GetSingleton().addConstraint(settings.Create(*prev, segment));

                prev = &segment;
            }

            group_id++;
        }

        {
            // Two light bodies attached to a heavy body (gives issues if the wrong anchor point is chosen)
            Body *light1 = body_interface.CreateBody(
                    BodyCreationSettings(new BoxShape(Vec3::sReplicate(0.1f)), Vec3(-5.0f, 7.0f, -5.2f),
                                         Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
            light1->SetCollisionGroup(CollisionGroup(group_filter, group_id, 0));
            body_interface.AddBody(light1->GetID(), EActivation::Activate);
            Body *heavy = body_interface.CreateBody(
                    BodyCreationSettings(new BoxShape(Vec3::sReplicate(5.0f)), Vec3(-5.0f, 7.0f, 0.0f),
                                         Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
            heavy->SetCollisionGroup(CollisionGroup(group_filter, group_id, 1));
            body_interface.AddBody(heavy->GetID(), EActivation::Activate);
            Body *light2 = body_interface.CreateBody(
                    BodyCreationSettings(new BoxShape(Vec3::sReplicate(0.1f)), Vec3(-5.0f, 7.0f, 5.2f),
                                         Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
            light2->SetCollisionGroup(CollisionGroup(group_filter, group_id, 2));
            body_interface.AddBody(light2->GetID(), EActivation::Activate);
            ++group_id;

            // Note: This violates the recommendation that body 1 is heavier than body 2, therefore this constraint will
            // not work well (the rotation constraint will not be solved accurately)
            SliderConstraintSettings slider1;
            slider1.mAutoDetectPoint = true;
            slider1.SetSliderAxis(Vec3::sAxisZ());
            slider1.mLimitsMin = 0.0f;
            slider1.mLimitsMax = 1.0f;
            PhysicsManager::GetSingleton().addConstraint(slider1.Create(*light1, *heavy));

            // This constraint has the heavy body as body 1 so will work fine
            SliderConstraintSettings slider2;
            slider2.mAutoDetectPoint = true;
            slider2.SetSliderAxis(Vec3::sAxisZ());
            slider2.mLimitsMin = 0.0f;
            slider2.mLimitsMax = 1.0f;
            PhysicsManager::GetSingleton().addConstraint(slider2.Create(*heavy, *light2));
        }

        {
            // Two bodies vertically stacked with a slider constraint
            Body *vert1 = body_interface.CreateBody(
                    BodyCreationSettings(new BoxShape(Vec3::sReplicate(1.0f)), Vec3(5, 9, 0), Quat::sIdentity(),
                                         EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
            vert1->SetCollisionGroup(CollisionGroup(group_filter, group_id, 0));
            body_interface.AddBody(vert1->GetID(), EActivation::Activate);
            Body *vert2 = body_interface.CreateBody(
                    BodyCreationSettings(new BoxShape(Vec3::sReplicate(1.0f)), Vec3(5, 3, 0), Quat::sIdentity(),
                                         EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
            vert2->SetCollisionGroup(CollisionGroup(group_filter, group_id, 1));
            body_interface.AddBody(vert2->GetID(), EActivation::Activate);
            ++group_id;

            SliderConstraintSettings slider;
            slider.mAutoDetectPoint = true;
            slider.SetSliderAxis(Vec3::sAxisY());
            slider.mLimitsMin = 0.0f;
            slider.mLimitsMax = 2.0f;
            PhysicsManager::GetSingleton().addConstraint(slider.Create(*vert1, *vert2));
        }

        {
            // Two bodies vertically stacked with a slider constraint using soft limits
            Body *vert1 = body_interface.CreateBody(
                    BodyCreationSettings(new BoxShape(Vec3::sReplicate(1.0f)), Vec3(10, 9, 0), Quat::sIdentity(),
                                         EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
            vert1->SetCollisionGroup(CollisionGroup(group_filter, group_id, 0));
            body_interface.AddBody(vert1->GetID(), EActivation::Activate);
            Body *vert2 = body_interface.CreateBody(
                    BodyCreationSettings(new BoxShape(Vec3::sReplicate(1.0f)), Vec3(10, 3, 0), Quat::sIdentity(),
                                         EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
            vert2->SetCollisionGroup(CollisionGroup(group_filter, group_id, 1));
            body_interface.AddBody(vert2->GetID(), EActivation::Activate);
            ++group_id;

            SliderConstraintSettings slider;
            slider.mAutoDetectPoint = true;
            slider.SetSliderAxis(Vec3::sAxisY());
            slider.mLimitsMin = 0.0f;
            slider.mLimitsMax = 2.0f;
            slider.mFrequency = 1.0f;
            slider.mDamping = 0.5f;
            PhysicsManager::GetSingleton().addConstraint(slider.Create(*vert1, *vert2));
        }
    }
    scene->play();
}

}  // namespace vox
