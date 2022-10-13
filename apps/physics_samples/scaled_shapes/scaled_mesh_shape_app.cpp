//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/scaled_shapes/scaled_mesh_shape_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include "apps/physics_samples/physics_utils.h"
#include "vox.math/perlin.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.toolkit/controls/orbit_control.h"
#include "vox.toolkit/physics_view/physics_view_subpass.h"

namespace vox {
namespace {
class ShowScript : public Script {
public:
    physics_view::PhysicsViewSubpass *_debugger{nullptr};
    JPH::BodyManager::DrawSettings inSettings;

    explicit ShowScript(Entity *entity) : Script(entity) { inSettings.mDrawShape = true; }

    void onPhysicsUpdate() override {
        _debugger->Clear();
        PhysicsManager::GetSingleton().drawBodies(inSettings, _debugger);
    }
};

}  // namespace

bool PhysicsScaledMeshShapeApp::prepare(Platform &platform) {
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

void PhysicsScaledMeshShapeApp::loadScene() {
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
        PhysicsUtils::createFloor(body_interface);

        const int n = 10;
        const float cell_size = 2.0f;
        const float max_height = 4.0f;

        // Create heights
        float heights[n + 1][n + 1];
        for (int x = 0; x <= n; ++x)
            for (int z = 0; z <= n; ++z)
                heights[x][z] = max_height * perlinNoise3(float(x) / n, 0, float(z) / n, 256, 256, 256);

        // Create 'wall' around grid
        for (int x = 0; x <= n; ++x) {
            heights[x][0] += 2.0f;
            heights[x][n] += 2.0f;
        }

        for (int y = 1; y < n; ++y) {
            heights[0][y] += 2.0f;
            heights[n][y] += 2.0f;
        }

        // Create regular grid of triangles
        TriangleList triangles;
        for (int x = 0; x < n; ++x)
            for (int z = 0; z < n; ++z) {
                float center = n * cell_size / 2;

                float x1 = cell_size * x - center;
                float z1 = cell_size * z - center;
                float x2 = x1 + cell_size;
                float z2 = z1 + cell_size;

                Float3 v1 = Float3(x1, heights[x][z], z1);
                Float3 v2 = Float3(x2, heights[x + 1][z], z1);
                Float3 v3 = Float3(x1, heights[x][z + 1], z2);
                Float3 v4 = Float3(x2, heights[x + 1][z + 1], z2);

                triangles.push_back(Triangle(v1, v3, v4));
                triangles.push_back(Triangle(v1, v4, v2));
            }

        RefConst<ShapeSettings> mesh_shape = new MeshShapeSettings(triangles);

        // Original shape
        Body &body1 = *body_interface.CreateBody(BodyCreationSettings(mesh_shape, Vec3(-60, 10, 0), Quat::sIdentity(),
                                                                      EMotionType::Static,
                                                                      PhysicsManager::Layers::NON_MOVING));
        body_interface.AddBody(body1.GetID(), EActivation::DontActivate);

        // Uniformly scaled shape < 1
        Body &body2 = *body_interface.CreateBody(
                BodyCreationSettings(new ScaledShapeSettings(mesh_shape, Vec3::sReplicate(0.5f)), Vec3(-40, 10, 0),
                                     Quat::sIdentity(), EMotionType::Static, PhysicsManager::Layers::NON_MOVING));
        body_interface.AddBody(body2.GetID(), EActivation::DontActivate);

        // Uniformly scaled shape > 1
        Body &body3 = *body_interface.CreateBody(
                BodyCreationSettings(new ScaledShapeSettings(mesh_shape, Vec3::sReplicate(1.5f)), Vec3(-20, 10, 0),
                                     Quat::sIdentity(), EMotionType::Static, PhysicsManager::Layers::NON_MOVING));
        body_interface.AddBody(body3.GetID(), EActivation::DontActivate);

        // Non-uniform scaled shape
        Body &body4 = *body_interface.CreateBody(
                BodyCreationSettings(new ScaledShapeSettings(mesh_shape, Vec3(0.5f, 1.0f, 1.5f)), Vec3(0, 10, 0),
                                     Quat::sIdentity(), EMotionType::Static, PhysicsManager::Layers::NON_MOVING));
        body_interface.AddBody(body4.GetID(), EActivation::DontActivate);

        // Flipped in 2 axis
        Body &body5 = *body_interface.CreateBody(
                BodyCreationSettings(new ScaledShapeSettings(mesh_shape, Vec3(-0.5f, 1.0f, -1.5f)), Vec3(20, 10, 0),
                                     Quat::sIdentity(), EMotionType::Static, PhysicsManager::Layers::NON_MOVING));
        body_interface.AddBody(body5.GetID(), EActivation::DontActivate);

        // Inside out
        Body &body6 = *body_interface.CreateBody(
                BodyCreationSettings(new ScaledShapeSettings(mesh_shape, Vec3(-0.5f, 1.0f, 1.5f)), Vec3(40, 10, 0),
                                     Quat::sIdentity(), EMotionType::Static, PhysicsManager::Layers::NON_MOVING));
        body_interface.AddBody(body6.GetID(), EActivation::DontActivate);

        // Upside down
        Body &body7 = *body_interface.CreateBody(
                BodyCreationSettings(new ScaledShapeSettings(mesh_shape, Vec3(0.5f, -1.0f, 1.5f)), Vec3(60, 10, 0),
                                     Quat::sIdentity(), EMotionType::Static, PhysicsManager::Layers::NON_MOVING));
        body_interface.AddBody(body7.GetID(), EActivation::DontActivate);

        // Create a number of balls above the meshes
        RefConst<Shape> sphere_shape = new SphereShape(0.2f);
        RefConst<Shape> box_shape = new BoxShape(Vec3(0.2f, 0.2f, 0.4f), 0.01f);
        for (int i = 0; i < 7; ++i)
            for (int j = 0; j < 5; ++j) {
                Body &dynamic = *body_interface.CreateBody(BodyCreationSettings(
                        (j & 1) ? box_shape : sphere_shape, Vec3(-60.0f + 20.0f * i, 10.0f + max_height + 0.5f * j, 0),
                        Quat::sIdentity(), EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
                body_interface.AddBody(dynamic.GetID(), EActivation::Activate);
            }
    }
    scene->play();
}

}  // namespace vox
