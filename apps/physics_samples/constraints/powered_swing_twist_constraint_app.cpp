//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/constraints/powered_swing_twist_constraint_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/GroupFilterTable.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

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

Vec3 PhysicsPoweredSwingTwistConstraintApp::sBodyRotation[] = {Vec3::sZero(), Vec3::sZero()};

bool PhysicsPoweredSwingTwistConstraintApp::prepare(Platform& platform) {
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

void PhysicsPoweredSwingTwistConstraintApp::loadScene() {
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

        float half_box_height = 1.5f;
        RefConst<Shape> box = new BoxShape(Vec3(0.25f, half_box_height, 0.5f));
        Quat body1_rotation = Quat::sEulerAngles(sBodyRotation[0]);
        Quat body2_rotation = Quat::sEulerAngles(sBodyRotation[1]) * body1_rotation;

        Vec3 body1_position(0, 20, 0);
        Body& body1 = *body_interface.CreateBody(BodyCreationSettings(
                box, body1_position, body1_rotation, EMotionType::Static, PhysicsManager::Layers::NON_MOVING));
        body1.SetCollisionGroup(CollisionGroup(group_filter, 0, 0));
        body_interface.AddBody(body1.GetID(), EActivation::DontActivate);

        Vec3 constraint_position = body1_position + body1_rotation * Vec3(0, -half_box_height, 0);

        Vec3 body2_position = constraint_position + body2_rotation * Vec3(0, -half_box_height, 0);
        Body& body2 = *body_interface.CreateBody(BodyCreationSettings(
                box, body2_position, body2_rotation, EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
        body2.SetCollisionGroup(CollisionGroup(group_filter, 0, 0));
        body2.GetMotionProperties()->SetLinearDamping(0.0f);
        body2.GetMotionProperties()->SetAngularDamping(0.0f);
        body2.SetAllowSleeping(false);
        body_interface.AddBody(body2.GetID(), EActivation::Activate);

        Ref<SwingTwistConstraintSettings> settings = new SwingTwistConstraintSettings;
        settings->mNormalHalfConeAngle = sNormalHalfConeAngle;
        settings->mPlaneHalfConeAngle = sPlaneHalfConeAngle;
        settings->mTwistMinAngle = sTwistMinAngle;
        settings->mTwistMaxAngle = sTwistMaxAngle;

        settings->mPosition1 = settings->mPosition2 = constraint_position;
        settings->mTwistAxis1 = settings->mTwistAxis2 = -body1_rotation.RotateAxisY();
        settings->mPlaneAxis1 = settings->mPlaneAxis2 = body1_rotation.RotateAxisX();

        mConstraint = static_cast<SwingTwistConstraint*>(settings->Create(body1, body2));
        PhysicsManager::GetSingleton().addConstraint(mConstraint);

        // Calculate inertia along the axis of the box, so that the acceleration of the motor / friction are correct for
        // twist
        Mat44 body2_inertia = body2.GetMotionProperties()->GetLocalSpaceInverseInertia().Inversed3x3();
        mInertiaBody2AsSeenFromConstraint = (body2_inertia * Vec3::sAxisY()).Length();
    }
    scene->play();
}

}  // namespace vox
