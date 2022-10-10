//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/general/damping_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

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

bool PhysicsDampingApp::prepare(Platform& platform) {
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

void PhysicsDampingApp::loadScene() {
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

        RefConst<Shape> sphere = new SphereShape(2.0f);

        // Bodies with increasing damping
        for (int i = 0; i <= 10; ++i) {
            Body& body = *body_interface.CreateBody(BodyCreationSettings(sphere, Vec3(-50.0f + i * 10.0f, 2.0f, -80.0f),
                                                                         Quat::sIdentity(), EMotionType::Dynamic,
                                                                         PhysicsManager::Layers::MOVING));
            body.GetMotionProperties()->SetAngularDamping(0.0f);
            body.GetMotionProperties()->SetLinearDamping(0.1f * i);
            body.SetLinearVelocity(Vec3(0, 0, 10));
            body_interface.AddBody(body.GetID(), EActivation::Activate);
        }

        for (int i = 0; i <= 10; ++i) {
            Body& body = *body_interface.CreateBody(BodyCreationSettings(sphere, Vec3(-50.0f + i * 10.0f, 2.0f, -90.0f),
                                                                         Quat::sIdentity(), EMotionType::Dynamic,
                                                                         PhysicsManager::Layers::MOVING));
            body.GetMotionProperties()->SetLinearDamping(0.0f);
            body.GetMotionProperties()->SetAngularDamping(0.1f * i);
            body.SetAngularVelocity(Vec3(0, 10, 0));
            body_interface.AddBody(body.GetID(), EActivation::Activate);
        }
    }
    scene->play();
}

}  // namespace vox
