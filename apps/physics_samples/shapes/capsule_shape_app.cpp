//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/shapes/capsule_shape_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
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
    }
};

}  // namespace

bool PhysicsCapsuleShapeApp::prepare(Platform& platform) {
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

void PhysicsCapsuleShapeApp::loadScene() {
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

        RefConst<Shape> big_capsule = new CapsuleShape(2.5f, 2);

        // Capsule on outer sphere
        Body& body1 = *body_interface.CreateBody(BodyCreationSettings(
                big_capsule, Vec3(0, 10, 0), Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
        body_interface.AddBody(body1.GetID(), EActivation::Activate);

        // Capsule on cylinder
        Body& body2 = *body_interface.CreateBody(
                BodyCreationSettings(big_capsule, Vec3(10, 10, 0), Quat::sRotation(Vec3::sAxisX(), 0.5f * JPH_PI),
                                     EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
        body_interface.AddBody(body2.GetID(), EActivation::Activate);

        RefConst<Shape> long_capsule = new CapsuleShape(5, 1);

        // Tower of capsules
        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 2; ++j) {
                Vec3 position{};
                Quat rotation{};
                if (i & 1) {
                    position = Vec3(-4.0f + 8.0f * j, 2.0f + 3.0f * i, -20.0f);
                    rotation = Quat::sRotation(Vec3::sAxisX(), 0.5f * JPH_PI);
                } else {
                    position = Vec3(0, 2.0f + 3.0f * i, -20.0f - 4.0f + 8.0f * j);
                    rotation = Quat::sRotation(Vec3::sAxisZ(), 0.5f * JPH_PI);
                }
                Body& body = *body_interface.CreateBody(BodyCreationSettings(
                        long_capsule, position, rotation, EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
                body_interface.AddBody(body.GetID(), EActivation::Activate);
            }
        }
    }
    scene->play();
}

}  // namespace vox
