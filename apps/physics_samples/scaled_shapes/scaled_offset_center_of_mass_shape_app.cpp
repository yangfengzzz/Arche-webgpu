//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/scaled_shapes/scaled_offset_center_of_mass_shape_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/OffsetCenterOfMassShape.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>

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
    }
};

}  // namespace

bool PhysicsScaledOffsetCenterOfMassShapeApp::prepare(Platform &platform) {
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

void PhysicsScaledOffsetCenterOfMassShapeApp::loadScene() {
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
        Body &floor = PhysicsUtils::createFloor(body_interface);
        floor.SetFriction(1.0f);

        Ref<ShapeSettings> cylinder = new CylinderShapeSettings(1.0f, 0.1f);
        Ref<OffsetCenterOfMassShapeSettings> top = new OffsetCenterOfMassShapeSettings(Vec3(0, 1, 0), cylinder);
        Ref<OffsetCenterOfMassShapeSettings> bottom = new OffsetCenterOfMassShapeSettings(Vec3(0, -1, 0), cylinder);

        // Initial body rotation
        Quat rotation = Quat::sRotation(Vec3::sAxisZ(), 0.4f * JPH_PI);

        // Cylinder with center of mass moved to the top side
        Body &body_top = *body_interface.CreateBody(
                BodyCreationSettings(new ScaledShapeSettings(top, Vec3(2.0f, 1.0f, 2.0f)), Vec3(-5, 5, 0), rotation,
                                     EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
        body_top.SetFriction(1.0f);
        body_interface.AddBody(body_top.GetID(), EActivation::Activate);

        // Cylinder with center of mass centered
        Body &body_center = *body_interface.CreateBody(
                BodyCreationSettings(new ScaledShapeSettings(cylinder, Vec3(2.0f, 1.0f, 2.0f)), Vec3(0, 5, 0), rotation,
                                     EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
        body_center.SetFriction(1.0f);
        body_interface.AddBody(body_center.GetID(), EActivation::Activate);

        // Cylinder with center of mass moved to the bottom side
        Body &body_bottom = *body_interface.CreateBody(
                BodyCreationSettings(new ScaledShapeSettings(bottom, Vec3(2.0f, 1.0f, 2.0f)), Vec3(5, 5, 0), rotation,
                                     EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
        body_bottom.SetFriction(1.0f);
        body_interface.AddBody(body_bottom.GetID(), EActivation::Activate);
    }
    scene->play();
}

}  // namespace vox
