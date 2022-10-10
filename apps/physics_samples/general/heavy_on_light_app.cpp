//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/general/heavy_on_light_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
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

bool PhysicsHeavyOnLightApp::prepare(Platform& platform) {
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

void PhysicsHeavyOnLightApp::loadScene() {
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

        Ref<BoxShape> box = new BoxShape(Vec3::sReplicate(5));
        box->SetDensity(1000.0f);

        for (int i = 1; i <= 10; ++i) {
            Body& body1 = *body_interface.CreateBody(BodyCreationSettings(box, Vec3(-75.0f + i * 15.0f, 10.0f, 0.0f),
                                                                          Quat::sIdentity(), EMotionType::Dynamic,
                                                                          PhysicsManager::Layers::MOVING));
            body_interface.AddBody(body1.GetID(), EActivation::Activate);

            Ref<BoxShape> box2 = new BoxShape(Vec3::sReplicate(5));
            box2->SetDensity(5000.0f * i);

            Body& body2 = *body_interface.CreateBody(BodyCreationSettings(box2, Vec3(-75.0f + i * 15.0f, 30.0f, 0.0f),
                                                                          Quat::sIdentity(), EMotionType::Dynamic,
                                                                          PhysicsManager::Layers::MOVING));
            body_interface.AddBody(body2.GetID(), EActivation::Activate);
        }
    }
    scene->play();
}

}  // namespace vox
