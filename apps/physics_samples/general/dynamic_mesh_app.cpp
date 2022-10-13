//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/general/dynamic_mesh_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>

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

bool PhysicsDynamicMeshApp::prepare(Platform& platform) {
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

void PhysicsDynamicMeshApp::loadScene() {
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

        constexpr float cTorusRadius = 3.0f;
        constexpr float cTubeRadius = 1.0f;

        // Create torus
        RefConst<Shape> mesh_shape = PhysicsUtils::createTorusMesh(cTorusRadius, cTubeRadius);
        BodyCreationSettings settings(mesh_shape, Vec3(0, 10, 0), Quat::sIdentity(), EMotionType::Dynamic,
                                      PhysicsManager::Layers::MOVING);

        // Mesh cannot calculate its mass, we must provide it
        settings.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;
        settings.mMassPropertiesOverride.SetMassAndInertiaOfSolidBox(
                2.0f * Vec3(cTorusRadius, cTubeRadius, cTorusRadius), 1000.0f);

        body_interface.CreateAndAddBody(settings, EActivation::Activate);

        // Wall of blocks
        RefConst<Shape> box_shape = new BoxShape(Vec3::sReplicate(0.5f));
        for (int i = 0; i < 7; ++i)
            for (int j = i / 2; j < 7 - (i + 1) / 2; ++j) {
                Vec3 position(-3.5f + j * 1.0f + (i & 1 ? 0.5f : 0.0f), 0.5f + i * 1.0f, 0.0f);
                Body& wall = *body_interface.CreateBody(BodyCreationSettings(
                        box_shape, position, Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
                body_interface.AddBody(wall.GetID(), EActivation::Activate);
            }
    }
    scene->play();
}

}  // namespace vox
