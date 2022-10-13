//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/shapes/offset_center_of_mass_shape_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/OffsetCenterOfMassShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

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
    }
};

}  // namespace

bool PhysicsOffsetCenterOfMassShapeApp::prepare(Platform& platform) {
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

void PhysicsOffsetCenterOfMassShapeApp::loadScene() {
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
        // Floor
        Body& floor = PhysicsUtils::createFloor(body_interface);
        floor.SetFriction(1.0f);

        Ref<ShapeSettings> sphere = new SphereShapeSettings(1.0f);
        Ref<OffsetCenterOfMassShapeSettings> left = new OffsetCenterOfMassShapeSettings(Vec3(-1, 0, 0), sphere);
        Ref<OffsetCenterOfMassShapeSettings> right = new OffsetCenterOfMassShapeSettings(Vec3(1, 0, 0), sphere);

        // Sphere with center of mass moved to the left side
        Body& body_left = *body_interface.CreateBody(BodyCreationSettings(
                left, Vec3(-5, 5, 0), Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
        body_left.SetFriction(1.0f);
        body_interface.AddBody(body_left.GetID(), EActivation::Activate);

        // Sphere with center of mass centered
        Body& body_center = *body_interface.CreateBody(BodyCreationSettings(
                sphere, Vec3(0, 5, 0), Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
        body_center.SetFriction(1.0f);
        body_interface.AddBody(body_center.GetID(), EActivation::Activate);

        // Sphere with center of mass moved to the right side
        Body& body_right = *body_interface.CreateBody(BodyCreationSettings(
                right, Vec3(5, 5, 0), Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
        body_right.SetFriction(1.0f);
        body_interface.AddBody(body_right.GetID(), EActivation::Activate);
    }
    scene->play();
}

}  // namespace vox
