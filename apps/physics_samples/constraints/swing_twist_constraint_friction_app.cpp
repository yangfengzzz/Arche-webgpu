//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/constraints/swing_twist_constraint_friction_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/GroupFilterTable.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>

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

bool PhysicsSwingTwistConstraintFrictionApp::prepare(Platform& platform) {
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

void PhysicsSwingTwistConstraintFrictionApp::loadScene() {
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

        float half_cylinder_height = 1.5f;
        RefConst<Shape> capsule = new CapsuleShape(half_cylinder_height, 0.5f);

        Vec3 body1_position(0, 10, 0);
        Body& body1 = *body_interface.CreateBody(BodyCreationSettings(
                capsule, body1_position, Quat::sIdentity(), EMotionType::Static, PhysicsManager::Layers::NON_MOVING));
        body1.SetCollisionGroup(CollisionGroup(group_filter, 0, 0));
        body_interface.AddBody(body1.GetID(), EActivation::DontActivate);

        Vec3 body2_position = body1_position + Vec3(0, -2.0f * half_cylinder_height, 0);
        Body& body2 = *body_interface.CreateBody(BodyCreationSettings(
                capsule, body2_position, Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
        body2.SetCollisionGroup(CollisionGroup(group_filter, 0, 0));
        body2.GetMotionProperties()->SetLinearDamping(0.0f);
        body2.GetMotionProperties()->SetAngularDamping(0.0f);
        body2.SetAllowSleeping(false);
        body_interface.AddBody(body2.GetID(), EActivation::Activate);

        SwingTwistConstraintSettings settings;
        settings.mPosition1 = settings.mPosition2 = body1_position + Vec3(0, -half_cylinder_height, 0);
        settings.mTwistAxis1 = settings.mTwistAxis2 = Vec3(0, -1, 0);
        settings.mPlaneAxis1 = settings.mPlaneAxis2 = Vec3::sAxisX();
        settings.mNormalHalfConeAngle = DegreesToRadians(90);
        settings.mPlaneHalfConeAngle = DegreesToRadians(90);
        settings.mTwistMinAngle = -JPH_PI;
        settings.mTwistMaxAngle = JPH_PI;

        float body2_inertia =
                (body2.GetMotionProperties()->GetLocalSpaceInverseInertia().Inversed3x3() * Vec3::sAxisY()).Length();
        constexpr float max_angular_acceleration = DegreesToRadians(90.0f);  // rad/s^2
        settings.mMaxFrictionTorque = body2_inertia * max_angular_acceleration;

        settings.mTwistMotorSettings = settings.mSwingMotorSettings = MotorSettings(10.0f, 2.0f);

        mConstraint = static_cast<SwingTwistConstraint*>(settings.Create(body1, body2));
        PhysicsManager::GetSingleton().addConstraint(mConstraint);
    }
    scene->play();
}

}  // namespace vox
