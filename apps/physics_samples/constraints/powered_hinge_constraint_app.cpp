//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/constraints/powered_hinge_constraint_app.h"

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

bool PhysicsPowerHingeConstraintApp::prepare(Platform& platform) {
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

void PhysicsPowerHingeConstraintApp::loadScene() {
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

        Vec3 body1_position(0, 10, 0);
        Body& body1 = *body_interface.CreateBody(BodyCreationSettings(
                box, body1_position, Quat::sIdentity(), EMotionType::Static, PhysicsManager::Layers::NON_MOVING));
        body1.SetCollisionGroup(CollisionGroup(group_filter, 0, 0));
        body_interface.AddBody(body1.GetID(), EActivation::DontActivate);

        Vec3 body2_position = body1_position + Vec3(box_size, 0, 0);
        Body& body2 = *body_interface.CreateBody(BodyCreationSettings(
                box, body2_position, Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
        body2.SetCollisionGroup(CollisionGroup(group_filter, 0, 0));
        body2.GetMotionProperties()->SetLinearDamping(0.0f);
        body2.GetMotionProperties()->SetAngularDamping(0.0f);
        body2.SetAllowSleeping(false);
        body_interface.AddBody(body2.GetID(), EActivation::Activate);

        Vec3 constraint_position = body1_position + Vec3(0.5f * box_size, 0, 0.5f * box_size);

        HingeConstraintSettings settings;
        settings.mPoint1 = settings.mPoint2 = constraint_position;
        settings.mHingeAxis1 = settings.mHingeAxis2 = Vec3::sAxisY();
        settings.mNormalAxis1 = settings.mNormalAxis2 = Vec3::sAxisX();
        mConstraint = static_cast<HingeConstraint*>(settings.Create(body1, body2));
        mConstraint->SetMotorState(EMotorState::Velocity);
        mConstraint->SetTargetAngularVelocity(DegreesToRadians(25));
        PhysicsManager::GetSingleton().addConstraint(mConstraint);

        // Calculate inertia of body 2 as seen from the constraint
        MassProperties body2_inertia_from_constraint;
        body2_inertia_from_constraint.mMass = 1.0f / body2.GetMotionProperties()->GetInverseMass();
        body2_inertia_from_constraint.mInertia =
                body2.GetMotionProperties()->GetLocalSpaceInverseInertia().Inversed3x3();
        body2_inertia_from_constraint.Translate(body2_position - constraint_position);
        mInertiaBody2AsSeenFromConstraint = (body2_inertia_from_constraint.mInertia * Vec3::sAxisY()).Length();
    }
    scene->play();
}

}  // namespace vox
