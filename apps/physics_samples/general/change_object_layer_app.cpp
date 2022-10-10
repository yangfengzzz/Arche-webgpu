//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/general/change_object_layer_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

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

bool PhysicsChangeObjectLayerApp::prepare(Platform& platform) {
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

void PhysicsChangeObjectLayerApp::loadScene() {
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

        // A dynamic box in the MOVING layer
        mMoving = body_interface.CreateAndAddBody(
                BodyCreationSettings(new BoxShape(Vec3(5, 0.1f, 5)), Vec3(0, 1.5f, 0), Quat::sIdentity(),
                                     EMotionType::Dynamic, PhysicsManager::Layers::MOVING),
                EActivation::Activate);

        // Lots of dynamic objects in the DEBRIS layer
        default_random_engine random;
        uniform_real_distribution<float> position_variation(-10, 10);
        for (int i = 0; i < 50; ++i) {
            Vec3 position(position_variation(random), 2.0f, position_variation(random));
            Quat rotation = Quat::sRandom(random);
            // mDebris.push_back(body_interface.CreateAndAddBody(
            //         BodyCreationSettings(new BoxShape(Vec3::sReplicate(0.1f)), position, rotation,
            //         EMotionType::Dynamic,
            //                              Layers::DEBRIS),
            //       EActivation::Activate));
        }
    }
    scene->play();
}

}  // namespace vox
