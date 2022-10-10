//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/scaled_shapes/scaled_triangle_shape_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/TriangleShape.h>

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

bool PhysicsScaledTriangleShapeApp::prepare(Platform &platform) {
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

void PhysicsScaledTriangleShapeApp::loadScene() {
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

        // Single triangle
        RefConst<TriangleShape> triangle_shape = new TriangleShape(Vec3(-10, -1, 0), Vec3(0, 1, 10), Vec3(10, -2, -10));

        // Original shape
        Body &body1 = *body_interface.CreateBody(BodyCreationSettings(triangle_shape, Vec3(-60, 10, 0),
                                                                      Quat::sIdentity(), EMotionType::Static,
                                                                      PhysicsManager::Layers::NON_MOVING));
        body_interface.AddBody(body1.GetID(), EActivation::DontActivate);

        // Uniformly scaled shape < 1
        Body &body2 = *body_interface.CreateBody(
                BodyCreationSettings(new ScaledShape(triangle_shape, Vec3::sReplicate(0.5f)), Vec3(-40, 10, 0),
                                     Quat::sIdentity(), EMotionType::Static, PhysicsManager::Layers::NON_MOVING));
        body_interface.AddBody(body2.GetID(), EActivation::DontActivate);

        // Uniformly scaled shape > 1
        Body &body3 = *body_interface.CreateBody(
                BodyCreationSettings(new ScaledShape(triangle_shape, Vec3::sReplicate(1.5f)), Vec3(-20, 10, 0),
                                     Quat::sIdentity(), EMotionType::Static, PhysicsManager::Layers::NON_MOVING));
        body_interface.AddBody(body3.GetID(), EActivation::DontActivate);

        // Non-uniform scaled shape
        Body &body4 = *body_interface.CreateBody(
                BodyCreationSettings(new ScaledShape(triangle_shape, Vec3(0.5f, 1.0f, 1.5f)), Vec3(0, 10, 0),
                                     Quat::sIdentity(), EMotionType::Static, PhysicsManager::Layers::NON_MOVING));
        body_interface.AddBody(body4.GetID(), EActivation::DontActivate);

        // Flipped in 2 axis
        Body &body5 = *body_interface.CreateBody(
                BodyCreationSettings(new ScaledShape(triangle_shape, Vec3(-0.5f, 1.0f, -1.5f)), Vec3(20, 10, 0),
                                     Quat::sIdentity(), EMotionType::Static, PhysicsManager::Layers::NON_MOVING));
        body_interface.AddBody(body5.GetID(), EActivation::DontActivate);

        // Inside out
        Body &body6 = *body_interface.CreateBody(
                BodyCreationSettings(new ScaledShape(triangle_shape, Vec3(-0.5f, 1.0f, 1.5f)), Vec3(40, 10, 0),
                                     Quat::sIdentity(), EMotionType::Static, PhysicsManager::Layers::NON_MOVING));
        body_interface.AddBody(body6.GetID(), EActivation::DontActivate);

        // Upside down
        Body &body7 = *body_interface.CreateBody(
                BodyCreationSettings(new ScaledShape(triangle_shape, Vec3(0.5f, -1.0f, 1.5f)), Vec3(60, 10, 0),
                                     Quat::sIdentity(), EMotionType::Static, PhysicsManager::Layers::NON_MOVING));
        body_interface.AddBody(body7.GetID(), EActivation::DontActivate);

        // Create a number of balls above the triangles
        RefConst<Shape> sphere_shape = new SphereShape(0.2f);
        RefConst<Shape> box_shape = new BoxShape(Vec3(0.2f, 0.2f, 0.4f), 0.01f);
        for (int i = 0; i < 7; ++i)
            for (int j = 0; j < 5; ++j) {
                Body &dynamic = *body_interface.CreateBody(BodyCreationSettings(
                        (j & 1) ? box_shape : sphere_shape, Vec3(-60.0f + 20.0f * i, 10.0f + 0.5f * j, 0),
                        Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
                body_interface.AddBody(dynamic.GetID(), EActivation::Activate);
            }
    }
    scene->play();
}

}  // namespace vox
