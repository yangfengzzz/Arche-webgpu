//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/general/kinematic_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
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

bool PhysicsKinematicApp::prepare(Platform& platform) {
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

void PhysicsKinematicApp::loadScene() {
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

        // Wall
        RefConst<Shape> box_shape = new BoxShape(Vec3(1.0f, 1.0f, 1.0f));
        for (int i = 0; i < 3; ++i)
            for (int j = i / 2; j < 10 - (i + 1) / 2; ++j) {
                Vec3 position(-10.0f + j * 2.0f + (i & 1 ? 1.0f : 0.0f), 1.0f + i * 2.0f, 0);
                Body& wall = *body_interface.CreateBody(BodyCreationSettings(
                        box_shape, position, Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
                body_interface.AddBody(wall.GetID(), EActivation::DontActivate);
            }

        // Kinematic object
        for (int i = 0; i < 2; ++i) {
            mKinematic[i] = body_interface.CreateBody(
                    BodyCreationSettings(new SphereShape(1.0f), Vec3(-10.0f, 2.0f, i == 0 ? 5.0f : -5.0f),
                                         Quat::sIdentity(), EMotionType::Kinematic, PhysicsManager::Layers::MOVING));
            body_interface.AddBody(mKinematic[i]->GetID(), EActivation::Activate);
        }
    }
    scene->play();
}

}  // namespace vox
