//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/constraints/powered_slider_constraint_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/GroupFilterTable.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

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

bool PhysicsPoweredSliderConstraintApp::prepare(Platform& platform) {
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

void PhysicsPoweredSliderConstraintApp::loadScene() {
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

        // Create group filter
        Ref<GroupFilterTable> group_filter = new GroupFilterTable;

        // Create box
        float box_size = 4.0f;
        RefConst<Shape> box = new BoxShape(Vec3::sReplicate(0.5f * box_size));

        Vec3 position(0, 10, 0);
        Body& body1 = *body_interface.CreateBody(BodyCreationSettings(
                box, position, Quat::sIdentity(), EMotionType::Static, PhysicsManager::Layers::NON_MOVING));
        body1.SetCollisionGroup(CollisionGroup(group_filter, 0, 0));
        body_interface.AddBody(body1.GetID(), EActivation::DontActivate);

        position += Vec3(box_size + 10.0f, 0, 0);

        mBody2 = body_interface.CreateBody(BodyCreationSettings(BodyCreationSettings(
                box, position, Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING)));
        mBody2->SetCollisionGroup(CollisionGroup(group_filter, 0, 0));
        mBody2->GetMotionProperties()->SetLinearDamping(0.0f);
        mBody2->SetAllowSleeping(false);
        body_interface.AddBody(mBody2->GetID(), EActivation::Activate);

        SliderConstraintSettings settings;
        settings.mAutoDetectPoint = true;
        settings.SetSliderAxis(Vec3::sAxisX());
        settings.mLimitsMin = -5.0f;
        settings.mLimitsMax = 100.0f;
        mConstraint = static_cast<SliderConstraint*>(settings.Create(body1, *mBody2));
        mConstraint->SetMotorState(EMotorState::Velocity);
        mConstraint->SetTargetVelocity(1);
        PhysicsManager::GetSingleton().addConstraint(mConstraint);
    }
    scene->play();
}

}  // namespace vox
