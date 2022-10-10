//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/general/center_of_mass_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>

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

bool PhysicsCenterOfMassApp::prepare(Platform& platform) {
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

void PhysicsCenterOfMassApp::loadScene() {
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

        // Compound shape with center of mass offset
        Ref<StaticCompoundShapeSettings> compound_shape1 = new StaticCompoundShapeSettings;
        compound_shape1->AddShape(Vec3(10, 0, 0), Quat::sIdentity(), new SphereShape(2));
        Body& body1 =
                *body_interface.CreateBody(BodyCreationSettings(compound_shape1, Vec3(0, 10.0f, 0), Quat::sIdentity(),
                                                                EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
        body_interface.AddBody(body1.GetID(), EActivation::Activate);

        // Create box with center of mass offset
        Array<Vec3> box;
        box.push_back(Vec3(10, 10, 10));
        box.push_back(Vec3(5, 10, 10));
        box.push_back(Vec3(10, 5, 10));
        box.push_back(Vec3(5, 5, 10));
        box.push_back(Vec3(10, 10, 5));
        box.push_back(Vec3(5, 10, 5));
        box.push_back(Vec3(10, 5, 5));
        box.push_back(Vec3(5, 5, 5));
        Body& body2 = *body_interface.CreateBody(BodyCreationSettings(
                new ConvexHullShapeSettings(box), Vec3(0, 10.0f, 20.0f),
                Quat::sRotation(Vec3::sAxisX(), 0.25f * JPH_PI), EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
        body_interface.AddBody(body2.GetID(), EActivation::Activate);

        // Compound
        Ref<StaticCompoundShapeSettings> compound_shape2 = new StaticCompoundShapeSettings;
        Quat rotation = Quat::sRotation(Vec3::sAxisX(), 0.25f * JPH_PI);
        compound_shape2->AddShape(Vec3(10, 0, 0), rotation, new CapsuleShape(5, 1));
        compound_shape2->AddShape(rotation * Vec3(10, -5, 0), Quat::sIdentity(), new SphereShape(4));
        compound_shape2->AddShape(rotation * Vec3(10, 5, 0), Quat::sIdentity(), new SphereShape(2));
        Body& body3 = *body_interface.CreateBody(BodyCreationSettings(compound_shape2, Vec3(0, 10.0f, 40.0f),
                                                                      Quat::sIdentity(), EMotionType::Dynamic,
                                                                      PhysicsManager::Layers::MOVING));
        body_interface.AddBody(body3.GetID(), EActivation::Activate);
    }
    scene->play();
}

}  // namespace vox
