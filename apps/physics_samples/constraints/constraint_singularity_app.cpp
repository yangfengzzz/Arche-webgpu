//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/constraints/constraint_singularity_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/GroupFilterTable.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Constraints/FixedConstraint.h>
#include <Jolt/Physics/Constraints/HingeConstraint.h>

#include "apps/physics_samples/physics_utils.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.toolkit/controls/orbit_control.h"
#include "vox.toolkit/physics_view/physics_view_subpass.h"

namespace vox {
namespace {
class ShowScript : public Script {
public:
    physics_view::PhysicsViewSubpass* _debugger{nullptr};
    JPH::BodyManager::DrawSettings inSettings;

    explicit ShowScript(Entity* entity) : Script(entity) { inSettings.mDrawShape = true; }

    void onPhysicsUpdate() override {
        _debugger->Clear();
        PhysicsManager::GetSingleton().drawBodies(inSettings, _debugger);
        PhysicsManager::GetSingleton().drawConstraints(_debugger);
    }
};

}  // namespace

bool PhysicsConstraintSingularityApp::prepare(Platform& platform) {
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

void PhysicsConstraintSingularityApp::loadScene() {
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
        PhysicsUtils::createFloor(body_interface);

        float box_size = 4.0f;
        RefConst<Shape> box = new BoxShape(Vec3::sReplicate(0.5f * box_size));

        const int num_constraint_types = 2;
        const int num_configurations = 4;

        // Create group filter
        Ref<GroupFilterTable> group_filter = new GroupFilterTable;
        CollisionGroup::GroupID group_id = 0;

        for (int constraint_type = 0; constraint_type < num_constraint_types; ++constraint_type)
            for (int configuration = 0; configuration < num_configurations; ++configuration) {
                Vec3 test_position(10.0f * constraint_type, 10.0f + 10.0f * configuration, 0);

                Body& body1 = *body_interface.CreateBody(BodyCreationSettings(box, test_position, Quat::sIdentity(),
                                                                              EMotionType::Static,
                                                                              PhysicsManager::Layers::NON_MOVING));
                body1.SetCollisionGroup(CollisionGroup(group_filter, group_id, 0));
                body_interface.AddBody(body1.GetID(), EActivation::DontActivate);

                Body& body2 = *body_interface.CreateBody(BodyCreationSettings(box, test_position + Vec3(box_size, 0, 0),
                                                                              Quat::sIdentity(), EMotionType::Dynamic,
                                                                              PhysicsManager::Layers::MOVING));
                body2.SetCollisionGroup(CollisionGroup(group_filter, group_id, 0));
                body_interface.AddBody(body2.GetID(), EActivation::Activate);

                Constraint* constraint;
                switch (constraint_type) {
                    case 0: {
                        HingeConstraintSettings settings;
                        settings.mPoint1 = settings.mPoint2 = test_position + Vec3(0.5f * box_size, 0, 0.5f * box_size);
                        settings.mHingeAxis1 = settings.mHingeAxis2 = Vec3::sAxisY();
                        settings.mNormalAxis1 = settings.mNormalAxis2 = Vec3::sAxisX();
                        settings.mLimitsMin = -0.01f;
                        settings.mLimitsMax = 0.01f;
                        constraint = settings.Create(body1, body2);
                        break;
                    }

                    default: {
                        FixedConstraintSettings settings;
                        settings.mAutoDetectPoint = true;
                        constraint = settings.Create(body1, body2);
                        break;
                    }
                }

                PhysicsManager::GetSingleton().addConstraint(constraint);

                Vec3 position;
                Quat orientation;
                switch (configuration) {
                    case 0:
                        position = test_position + Vec3(0, 0, box_size);
                        orientation = Quat::sRotation(Vec3::sAxisY(), DegreesToRadians(180.0f));
                        break;

                    case 1:
                        position = test_position + Vec3(0, 0, box_size);
                        orientation = Quat::sRotation(Vec3::sAxisY(), DegreesToRadians(-90.0f)) *
                                      Quat::sRotation(Vec3::sAxisX(), DegreesToRadians(180.0f));
                        break;

                    case 2:
                        position = test_position + Vec3(box_size, 0, 0);
                        orientation = Quat::sRotation(Vec3::sAxisY(), DegreesToRadians(90.0f)) *
                                      Quat::sRotation(Vec3::sAxisZ(), DegreesToRadians(90.0f));
                        break;

                    default:
                        JPH_ASSERT(configuration == 3);
                        position = test_position + Vec3(-box_size, 0, 0);
                        orientation = Quat::sRotation(Vec3::sAxisY(), DegreesToRadians(90.0f)) *
                                      Quat::sRotation(Vec3::sAxisZ(), DegreesToRadians(90.0f));
                        break;
                }

                body_interface.SetPositionAndRotation(body2.GetID(), position, orientation, EActivation::DontActivate);

                ++group_id;
            }
    }
    scene->play();
}

}  // namespace vox
