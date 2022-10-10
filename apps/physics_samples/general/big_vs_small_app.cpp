//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/general/big_vs_small_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>

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

bool PhysicsBigVsSmallApp::prepare(Platform& platform) {
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

void PhysicsBigVsSmallApp::loadScene() {
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
        // Create a big triangle
        TriangleList triangles;
        triangles.push_back(Triangle(Float3(-100, 0, 0), Float3(0, 0, 100), Float3(100, 0, -100)));
        Body& triangle = *body_interface.CreateBody(
                BodyCreationSettings(new MeshShapeSettings(triangles), Vec3::sZero(), Quat::sIdentity(),
                                     EMotionType::Static, PhysicsManager::Layers::NON_MOVING));
        body_interface.AddBody(triangle.GetID(), EActivation::DontActivate);

        // A small box
        Body& body = *body_interface.CreateBody(
                BodyCreationSettings(new BoxShape(Vec3(0.1f, 0.1f, 0.1f)), Vec3(0, 1.0f, 0), Quat::sIdentity(),
                                     EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
        body.SetAllowSleeping(false);
        body_interface.AddBody(body.GetID(), EActivation::Activate);
    }
    scene->play();
}

}  // namespace vox
