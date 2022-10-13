//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/constraints/six_dof_constraint_app.h"

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

float PhysicsSixDofConstraintApp::sLimitMin[EAxis::Num] = {0, 0, 0, 0, 0, 0};
float PhysicsSixDofConstraintApp::sLimitMax[EAxis::Num] = {0, 0, 0, 0, 0, 0};
bool PhysicsSixDofConstraintApp::sEnableLimits[EAxis::Num] = {true, true, true, true, true, true};

PhysicsSixDofConstraintApp::SettingsRef PhysicsSixDofConstraintApp::sSettings = []() {
    static SixDOFConstraintSettings settings;
    settings.SetEmbedded();
    settings.mAxisX1 = settings.mAxisX2 = -Vec3::sAxisY();
    settings.mAxisY1 = settings.mAxisY2 = Vec3::sAxisZ();
    for (int i = 0; i < 6; ++i) settings.mMotorSettings[i] = MotorSettings(10.0f, 2.0f);
    return &settings;
}();

bool PhysicsSixDofConstraintApp::prepare(Platform& platform) {
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

void PhysicsSixDofConstraintApp::loadScene() {
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

        // Convert limits to settings class
        for (int i = 0; i < EAxis::Num; ++i)
            if (sEnableLimits[i]) {
                if (sLimitMax[i] - sLimitMin[i] < 1.0e-3f)
                    sSettings->MakeFixedAxis((EAxis)i);
                else
                    sSettings->SetLimitedAxis((EAxis)i, sLimitMin[i], sLimitMax[i]);
            } else {
                sSettings->MakeFreeAxis((EAxis)i);
            }

        // Create group filter
        Ref<GroupFilterTable> group_filter = new GroupFilterTable;

        // Create box
        float half_box_height = 1.5f;
        Vec3 position(0, 25, 0);
        RefConst<BoxShape> box = new BoxShape(Vec3(0.5f, half_box_height, 0.25f));

        // Create static body
        Body& body1 = *body_interface.CreateBody(BodyCreationSettings(
                box, position, Quat::sIdentity(), EMotionType::Static, PhysicsManager::Layers::NON_MOVING));
        body1.SetCollisionGroup(CollisionGroup(group_filter, 0, 0));
        body_interface.AddBody(body1.GetID(), EActivation::DontActivate);

        // Create dynamic body
        Body& body2 = *body_interface.CreateBody(
                BodyCreationSettings(box, position - Vec3(0, 2.0f * half_box_height, 0), Quat::sIdentity(),
                                     EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
        body2.SetCollisionGroup(CollisionGroup(group_filter, 0, 0));
        body2.SetAllowSleeping(false);
        body_interface.AddBody(body2.GetID(), EActivation::Activate);

        // Set constraint position
        sSettings->mPosition1 = sSettings->mPosition2 = position - Vec3(0, half_box_height, 0);

        // Set force limits
        const float max_acceleration = 1.0f;
        for (int i = 0; i < 3; ++i)
            sSettings->mMotorSettings[i].SetForceLimit(max_acceleration /
                                                       body2.GetMotionProperties()->GetInverseMass());

        // Create constraint
        mConstraint = static_cast<SixDOFConstraint*>(sSettings->Create(body1, body2));
        PhysicsManager::GetSingleton().addConstraint(mConstraint);
    }
    scene->play();
}

}  // namespace vox
