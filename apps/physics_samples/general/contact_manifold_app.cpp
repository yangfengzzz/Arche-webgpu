//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/general/contact_manifold_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>

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

bool PhysicsContactManifoldApp::prepare(Platform& platform) {
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

void PhysicsContactManifoldApp::loadScene() {
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

        RefConst<Shape> big_box = new BoxShape(Vec3(4, 4, 4), 0.0f);
        RefConst<Shape> capsule = new CapsuleShape(5, 2);
        RefConst<Shape> long_box = new BoxShape(Vec3(2, 7, 2));

        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 2; ++j) {
                // Create a box
                Body& box = *body_interface.CreateBody(BodyCreationSettings(
                        big_box, Vec3(-20.0f + i * 10.0f, 4, -20.0f + j * 40.0f), Quat::sIdentity(),
                        EMotionType::Static, PhysicsManager::Layers::NON_MOVING));
                body_interface.AddBody(box.GetID(), EActivation::DontActivate);

                // Place a dynamic body on it
                Body& body = *body_interface.CreateBody(BodyCreationSettings(
                        j == 0 ? capsule : long_box, Vec3(-20.0f + i * 10.0f, 12, -5.0f + i * 5.0f - 20.0f + j * 40.0f),
                        Quat::sRotation(Vec3::sAxisY(), 0.1f * JPH_PI) * Quat::sRotation(Vec3::sAxisX(), 0.5f * JPH_PI),
                        EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
                body_interface.AddBody(body.GetID(), EActivation::Activate);
            }
    }
    scene->play();
}

}  // namespace vox
