//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/constraints/cone_constraint_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/GroupFilterTable.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Constraints/ConeConstraint.h>

#include "apps/physics_samples/physics_utils.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.toolkit/controls/orbit_control.h"
#include "vox.toolkit/physics_debugger/physics_debug_subpass.h"

namespace vox {
namespace {
class ShowScript : public Script {
public:
    physics_debugger::PhysicsDebugSubpass* _debugger{nullptr};
    JPH::BodyManager::DrawSettings inSettings;

    explicit ShowScript(Entity* entity) : Script(entity) { inSettings.mDrawShape = true; }

    void onPhysicsUpdate() override {
        _debugger->Clear();
        PhysicsManager::GetSingleton().drawBodies(inSettings, _debugger);
        PhysicsManager::GetSingleton().drawConstraints(_debugger);
    }
};

}  // namespace

bool PhysicsConeConstraintApp::prepare(Platform& platform) {
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

void PhysicsConeConstraintApp::loadScene() {
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

        float half_cylinder_height = 2.5f;

        const int cChainLength = 5;

        // Build a collision group filter that disables collision between adjacent bodies
        Ref<GroupFilterTable> group_filter = new GroupFilterTable(cChainLength);
        for (CollisionGroup::SubGroupID i = 0; i < cChainLength - 1; ++i) group_filter->DisableCollision(i, i + 1);

        // Bodies attached through cone constraints
        for (int j = 0; j < 2; ++j) {
            Body* prev = nullptr;
            Quat rotation = Quat::sRotation(Vec3::sAxisZ(), 0.5f * JPH_PI);
            Vec3 position(0, 20.0f, 10.0f * j);
            for (int i = 0; i < cChainLength; ++i) {
                position += Vec3(2.0f * half_cylinder_height, 0, 0);

                Body& segment = *body_interface.CreateBody(BodyCreationSettings(
                        new CapsuleShape(half_cylinder_height, 1), position,
                        Quat::sRotation(Vec3::sAxisX(), 0.25f * JPH_PI * i) * rotation,
                        i == 0 ? EMotionType::Static : EMotionType::Dynamic,
                        i == 0 ? PhysicsManager::Layers::NON_MOVING : PhysicsManager::Layers::MOVING));
                segment.SetCollisionGroup(
                        CollisionGroup(group_filter, CollisionGroup::GroupID(j), CollisionGroup::SubGroupID(i)));
                body_interface.AddBody(segment.GetID(), EActivation::Activate);

                if (prev != nullptr) {
                    ConeConstraintSettings settings;
                    settings.mPoint1 = settings.mPoint2 = position + Vec3(-half_cylinder_height, 0, 0);
                    settings.mTwistAxis1 = settings.mTwistAxis2 = Vec3(1, 0, 0);
                    if (j == 0)
                        settings.mHalfConeAngle = 0.0f;
                    else
                        settings.mHalfConeAngle = DegreesToRadians(20);

                    PhysicsManager::GetSingleton().addConstraint(settings.Create(*prev, segment));
                }

                prev = &segment;
            }
        }
    }
    scene->play();
}

}  // namespace vox
