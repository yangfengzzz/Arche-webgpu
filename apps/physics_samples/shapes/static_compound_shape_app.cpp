//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/shapes/static_compound_shape_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
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

bool PhysicsStaticCompoundShapeApp::prepare(Platform& platform) {
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

void PhysicsStaticCompoundShapeApp::loadScene() {
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
        // Floor
        PhysicsUtils::createFloor(body_interface);

        // Simple compound
        Ref<StaticCompoundShapeSettings> compound_shape1 = new StaticCompoundShapeSettings;
        compound_shape1->AddShape(Vec3::sZero(), Quat::sIdentity(), new CapsuleShape(5, 1));
        compound_shape1->AddShape(Vec3(0, -5, 0), Quat::sIdentity(), new SphereShape(2));
        compound_shape1->AddShape(Vec3(0, 5, 0), Quat::sIdentity(), new SphereShape(2));

        // Compound with sub compound and rotation
        Ref<StaticCompoundShapeSettings> sub_compound = new StaticCompoundShapeSettings;
        sub_compound->AddShape(Vec3(0, 1.5f, 0), Quat::sRotation(Vec3::sAxisZ(), 0.5f * JPH_PI),
                               new BoxShape(Vec3(1.5f, 0.25f, 0.2f)));
        sub_compound->AddShape(Vec3(1.5f, 0, 0), Quat::sRotation(Vec3::sAxisZ(), 0.5f * JPH_PI),
                               new CylinderShape(1.5f, 0.2f));
        sub_compound->AddShape(Vec3(0, 0, 1.5f), Quat::sRotation(Vec3::sAxisX(), 0.5f * JPH_PI),
                               new TaperedCapsuleShapeSettings(1.5f, 0.25f, 0.2f));

        Ref<StaticCompoundShapeSettings> compound_shape2 = new StaticCompoundShapeSettings;
        compound_shape2->AddShape(
                Vec3(0, 0, 0),
                Quat::sRotation(Vec3::sAxisX(), -0.25f * JPH_PI) * Quat::sRotation(Vec3::sAxisZ(), 0.25f * JPH_PI),
                sub_compound);
        compound_shape2->AddShape(
                Vec3(0, -0.1f, 0),
                Quat::sRotation(Vec3::sAxisX(), 0.25f * JPH_PI) * Quat::sRotation(Vec3::sAxisZ(), -0.75f * JPH_PI),
                sub_compound);

        // Compound with large amount of sub shapes
        Ref<StaticCompoundShapeSettings> compound_shape3 = new StaticCompoundShapeSettings;
        for (int y = -2; y <= 2; ++y)
            for (int x = -2; x <= 2; ++x)
                for (int z = -2; z <= 2; ++z)
                    compound_shape3->AddShape(Vec3(0.5f * x, 0.5f * y, 0.5f * z),
                                              Quat::sRotation(Vec3::sAxisX(), -0.25f * JPH_PI) *
                                                      Quat::sRotation(Vec3::sAxisZ(), 0.25f * JPH_PI),
                                              new BoxShape(Vec3::sReplicate(0.5f)));

        Ref<StaticCompoundShapeSettings> shapes[] = {compound_shape1, compound_shape2, compound_shape3};

        for (int i = 0; i < 10; ++i)
            for (int j = 0; j < 3; ++j) {
                Quat rotation;
                if ((i & 1) == 0)
                    rotation = Quat::sRotation(Vec3::sAxisX(), 0.5f * JPH_PI);
                else
                    rotation = Quat::sRotation(Vec3::sAxisZ(), 0.5f * JPH_PI);
                Body& body = *body_interface.CreateBody(
                        BodyCreationSettings(shapes[j], Vec3(0, 10.0f + 4.0f * i, j * 20.0f), rotation,
                                             EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
                body_interface.AddBody(body.GetID(), EActivation::Activate);
            }
    }
    scene->play();
}

}  // namespace vox
