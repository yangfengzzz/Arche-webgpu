//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/general/manifold_reduction_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/PhysicsMaterialSimple.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>

#include "vox.math/perlin.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.toolkit/controls/orbit_control.h"
#include "vox.toolkit/physics_debugger/physics_debug_subpass.h"

namespace vox {
namespace {
class ShowScript : public Script {
public:
    physics_debugger::PhysicsDebugSubpass *_debugger{nullptr};
    JPH::BodyManager::DrawSettings inSettings;

    explicit ShowScript(Entity *entity) : Script(entity) { inSettings.mDrawShape = true; }

    void onPhysicsUpdate() override {
        _debugger->Clear();
        PhysicsManager::GetSingleton().drawBodies(inSettings, _debugger);
    }
};

}  // namespace

bool PhysicsManifoldReductionApp::prepare(Platform &platform) {
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

void PhysicsManifoldReductionApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    scene->ambientLight()->setDiffuseSolidColor(Color(1, 1, 1));
    auto rootEntity = scene->createRootEntity();

    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(30, 30, 30);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    cameraEntity->addComponent<control::OrbitControl>();

    JPH::BodyInterface &body_interface = PhysicsManager::GetSingleton().getBodyInterface();
    {
        constexpr float cPerturbance = 0.02f;

        // Create mesh of regular grid of triangles
        TriangleList triangles;
        for (int x = -10; x < 10; ++x)
            for (int z = -10; z < 10; ++z) {
                float x1 = 0.1f * x;
                float z1 = 0.1f * z;
                float x2 = x1 + 0.1f;
                float z2 = z1 + 0.1f;

                Float3 v1 = Float3(x1, cPerturbance * perlinNoise3(x1, 0, z1, 256, 256, 256), z1);
                Float3 v2 = Float3(x2, cPerturbance * perlinNoise3(x2, 0, z1, 256, 256, 256), z1);
                Float3 v3 = Float3(x1, cPerturbance * perlinNoise3(x1, 0, z2, 256, 256, 256), z2);
                Float3 v4 = Float3(x2, cPerturbance * perlinNoise3(x2, 0, z2, 256, 256, 256), z2);

                triangles.push_back(Triangle(v1, v3, v4, 0));
                triangles.push_back(Triangle(v1, v4, v2, 0));
            }
        PhysicsMaterialList materials;
        materials.push_back(new PhysicsMaterialSimple());
        Ref<ShapeSettings> mesh_shape = new MeshShapeSettings(triangles, materials);

        // Floor
        Body &floor = *body_interface.CreateBody(
                BodyCreationSettings(new ScaledShapeSettings(mesh_shape, Vec3::sReplicate(20)), Vec3::sZero(),
                                     Quat::sIdentity(), EMotionType::Static, PhysicsManager::Layers::NON_MOVING));
        body_interface.AddBody(floor.GetID(), EActivation::DontActivate);

        // Create a box made of meshes
        Ref<StaticCompoundShapeSettings> mesh_box_shape = new StaticCompoundShapeSettings;
        mesh_box_shape->AddShape(Vec3(0, -1, 0), Quat::sRotation(Vec3::sAxisX(), JPH_PI), mesh_shape);
        mesh_box_shape->AddShape(Vec3(0, 1, 0), Quat::sIdentity(), mesh_shape);
        mesh_box_shape->AddShape(Vec3(-1, 0, 0), Quat::sRotation(Vec3::sAxisZ(), 0.5f * JPH_PI), mesh_shape);
        mesh_box_shape->AddShape(Vec3(1, 0, 0), Quat::sRotation(Vec3::sAxisZ(), -0.5f * JPH_PI), mesh_shape);
        mesh_box_shape->AddShape(Vec3(0, 0, -1), Quat::sRotation(Vec3::sAxisX(), -0.5f * JPH_PI), mesh_shape);
        mesh_box_shape->AddShape(Vec3(0, 0, 1), Quat::sRotation(Vec3::sAxisX(), 0.5f * JPH_PI), mesh_shape);

        // A convex box
        RefConst<ShapeSettings> box_shape = new BoxShapeSettings(Vec3(1, 1, 1), 0.0f);

        {
            // Create a compound of 3 mesh boxes
            Ref<StaticCompoundShapeSettings> three_mesh_box_shape = new StaticCompoundShapeSettings;
            three_mesh_box_shape->AddShape(Vec3(-2.1f, 0, 0), Quat::sIdentity(), mesh_box_shape);
            three_mesh_box_shape->AddShape(Vec3(0, -1, 0), Quat::sIdentity(), mesh_box_shape);
            three_mesh_box_shape->AddShape(Vec3(2.1f, 0, 0), Quat::sIdentity(), mesh_box_shape);

            // Create a compound of 3 convex boxes
            Ref<StaticCompoundShapeSettings> three_box_shape = new StaticCompoundShapeSettings;
            three_box_shape->AddShape(Vec3(-2.1f, 0, 0), Quat::sIdentity(), box_shape);
            three_box_shape->AddShape(Vec3(0, -1.1f, 0), Quat::sIdentity(), box_shape);
            three_box_shape->AddShape(Vec3(2.1f, 0, 0), Quat::sIdentity(), box_shape);

            // A set of 3 mesh boxes to rest on
            Body &three_mesh_box = *body_interface.CreateBody(
                    BodyCreationSettings(three_mesh_box_shape, Vec3(0, 1, 0), Quat::sIdentity(), EMotionType::Static,
                                         PhysicsManager::Layers::NON_MOVING));
            body_interface.AddBody(three_mesh_box.GetID(), EActivation::DontActivate);

            // A set of 3 boxes that are dynamic where the middle one penetrates more than the other two
            BodyCreationSettings box_settings(three_box_shape, Vec3(0, 2.95f, 0), Quat::sIdentity(),
                                              EMotionType::Dynamic, PhysicsManager::Layers::MOVING);
            box_settings.mAllowSleeping = false;
            Body &box = *body_interface.CreateBody(box_settings);
            body_interface.AddBody(box.GetID(), EActivation::Activate);
        }

        {
            // Create a compound of 2 mesh boxes
            Ref<StaticCompoundShapeSettings> two_mesh_box_shape = new StaticCompoundShapeSettings;
            two_mesh_box_shape->AddShape(Vec3(-2.1f, 0, 0), Quat::sIdentity(), mesh_box_shape);
            two_mesh_box_shape->AddShape(Vec3(0, -1, 0), Quat::sIdentity(), mesh_box_shape);

            // Create a compound of 2 convex boxes
            Ref<StaticCompoundShapeSettings> two_box_shape = new StaticCompoundShapeSettings;
            two_box_shape->AddShape(Vec3(-2.1f, 0, 0), Quat::sIdentity(), box_shape);
            two_box_shape->AddShape(Vec3(0, -1, 0), Quat::sIdentity(), box_shape);

            // A set of 2 mesh boxes to rest on
            Body &two_mesh_box = *body_interface.CreateBody(BodyCreationSettings(two_mesh_box_shape, Vec3(0, 1, 4),
                                                                                 Quat::sIdentity(), EMotionType::Static,
                                                                                 PhysicsManager::Layers::NON_MOVING));
            body_interface.AddBody(two_mesh_box.GetID(), EActivation::DontActivate);

            // A set of 2 boxes that are dynamic, one is lower than the other
            BodyCreationSettings box_settings(two_box_shape, Vec3(0, 4, 4), Quat::sIdentity(), EMotionType::Dynamic,
                                              PhysicsManager::Layers::MOVING);
            box_settings.mAllowSleeping = false;
            Body &box = *body_interface.CreateBody(box_settings);
            body_interface.AddBody(box.GetID(), EActivation::Activate);
        }

        {
            // Create a compound of 2 meshes under small angle, small enough to combine the manifolds.
            Ref<StaticCompoundShapeSettings> two_mesh_shape = new StaticCompoundShapeSettings;
            two_mesh_shape->AddShape(Vec3(1, 0, 0), Quat::sRotation(Vec3::sAxisZ(), DegreesToRadians(2)), mesh_shape);
            two_mesh_shape->AddShape(Vec3(-1, 0, 0), Quat::sRotation(Vec3::sAxisZ(), DegreesToRadians(-2)), mesh_shape);

            // A set of 2 meshes to rest on
            Body &two_mesh_box = *body_interface.CreateBody(BodyCreationSettings(two_mesh_shape, Vec3(0, 1, -4),
                                                                                 Quat::sIdentity(), EMotionType::Static,
                                                                                 PhysicsManager::Layers::NON_MOVING));
            body_interface.AddBody(two_mesh_box.GetID(), EActivation::DontActivate);

            // A box that is dynamic, resting on the slightly sloped surface. The surface normals are close enough so
            // that the manifold should be merged.
            BodyCreationSettings box_settings(box_shape, Vec3(0, 4, -4), Quat::sIdentity(), EMotionType::Dynamic,
                                              PhysicsManager::Layers::MOVING);
            box_settings.mAllowSleeping = false;
            Body &box = *body_interface.CreateBody(box_settings);
            body_interface.AddBody(box.GetID(), EActivation::Activate);
        }

        {
            // Create a compound of 2 meshes under small angle, but bigger than the limit to combine the manifolds.
            Ref<StaticCompoundShapeSettings> two_mesh_shape = new StaticCompoundShapeSettings();
            two_mesh_shape->AddShape(Vec3(1, 0, 0), Quat::sRotation(Vec3::sAxisZ(), DegreesToRadians(3)), mesh_shape);
            two_mesh_shape->AddShape(Vec3(-1, 0, 0), Quat::sRotation(Vec3::sAxisZ(), DegreesToRadians(-3)), mesh_shape);

            // A set of 2 meshes to rest on
            Body &two_mesh_box = *body_interface.CreateBody(BodyCreationSettings(two_mesh_shape, Vec3(0, 1, -8),
                                                                                 Quat::sIdentity(), EMotionType::Static,
                                                                                 PhysicsManager::Layers::NON_MOVING));
            body_interface.AddBody(two_mesh_box.GetID(), EActivation::DontActivate);

            // A box that is dynamic, resting on the slightly sloped surface. The surface normals are not close enough
            // so that the manifold should be merged.
            BodyCreationSettings box_settings(box_shape, Vec3(0, 4, -8), Quat::sIdentity(), EMotionType::Dynamic,
                                              PhysicsManager::Layers::MOVING);
            box_settings.mAllowSleeping = false;
            Body &box = *body_interface.CreateBody(box_settings);
            body_interface.AddBody(box.GetID(), EActivation::Activate);
        }
    }
    scene->play();
}

}  // namespace vox
