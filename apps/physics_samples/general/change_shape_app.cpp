//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/general/change_shape_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <Jolt/Physics/Collision/Shape/TaperedCapsuleShape.h>

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

bool PhysicChangeShapeApp::prepare(Platform& platform) {
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

void PhysicChangeShapeApp::loadScene() {
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

        // Shapes
        mShapes.push_back(new BoxShape(Vec3(0.5f, 1.5f, 0.5f)));
        mShapes.push_back(new SphereShape(0.5f));
        mShapes.push_back(new CapsuleShape(1.0f, 0.5f));
        mShapes.push_back(TaperedCapsuleShapeSettings(1.0f, 0.5f, 0.3f).Create().Get());

        // Compound with center of mass shifted (this requires a correction of the position in the body)
        StaticCompoundShapeSettings compound_settings;
        compound_settings.AddShape(Vec3(0, 1.5f, 0), Quat::sIdentity(), new CapsuleShape(1.5f, 0.5f));
        compound_settings.AddShape(Vec3(0, 3, 0), Quat::sIdentity(), new SphereShape(1));
        RefConst<Shape> compound = compound_settings.Create().Get();
        mShapes.push_back(compound);

        // Create dynamic body that changes shape
        BodyCreationSettings settings;
        settings.SetShape(mShapes[mShapeIdx]);
        settings.mPosition = Vec3(0, 10, 0);
        settings.mMotionType = EMotionType::Dynamic;
        settings.mObjectLayer = PhysicsManager::Layers::MOVING;
        mBodyID = body_interface.CreateBody(settings)->GetID();
        body_interface.AddBody(mBodyID, EActivation::Activate);
    }
    scene->play();
}

}  // namespace vox
