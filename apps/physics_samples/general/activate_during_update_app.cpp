//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/general/activate_during_update_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
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
    }
};

}  // namespace

bool PhysicsActivateDuringUpdateApp::prepare(Platform& platform) {
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

void PhysicsActivateDuringUpdateApp::loadScene() {
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

        BodyCreationSettings settings;
        settings.SetShape(new BoxShape(Vec3::sReplicate(0.5f)));
        settings.mMotionType = EMotionType::Dynamic;
        settings.mObjectLayer = PhysicsManager::Layers::MOVING;

        const int cNumBodies = 3;
        const float cPenetrationSlop = PhysicsManager::GetSingleton().getPhysicsSettings().mPenetrationSlop;

        for (int i = 0; i < cNumBodies; ++i) {
            settings.mPosition = Vec3(i * (1.0f - cPenetrationSlop), 2.0f, 0);
            BodyID body_id = body_interface.CreateBody(settings)->GetID();
            body_interface.AddBody(body_id, EActivation::DontActivate);
            if (i == 0) body_interface.SetLinearVelocity(body_id, Vec3(500, 0, 0));
        }

        for (int i = 0; i < cNumBodies; ++i) {
            settings.mPosition = Vec3(i * (1.0f - cPenetrationSlop), 2.0f, 2.0f);
            BodyID body_id = body_interface.CreateBody(settings)->GetID();
            body_interface.AddBody(body_id, EActivation::DontActivate);
            if (i == cNumBodies - 1) body_interface.SetLinearVelocity(body_id, Vec3(-500, 0, 0));
        }
    }
    scene->play();
}

}  // namespace vox
