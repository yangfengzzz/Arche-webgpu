//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_samples/shapes/mesh_shape_app.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/PhysicsMaterialSimple.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>

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

bool PhysicsMeshShapeApp::prepare(Platform& platform) {
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

void PhysicsMeshShapeApp::loadScene() {
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
        // Create regular grid of triangles
        uint32 max_material_index = 0;
        TriangleList triangles;
        for (int x = -10; x < 10; ++x)
            for (int z = -10; z < 10; ++z) {
                float x1 = 10.0f * x;
                float z1 = 10.0f * z;
                float x2 = x1 + 10.0f;
                float z2 = z1 + 10.0f;

                Float3 v1 = Float3(x1, 0, z1);
                Float3 v2 = Float3(x2, 0, z1);
                Float3 v3 = Float3(x1, 0, z2);
                Float3 v4 = Float3(x2, 0, z2);

                uint32 material_index = uint32((Vec3(v1) + Vec3(v2) + Vec3(v3) + Vec3(v4)).Length() / 40.0f);
                max_material_index = std::max(max_material_index, material_index);
                triangles.push_back(Triangle(v1, v3, v4, material_index));
                triangles.push_back(Triangle(v1, v4, v2, material_index));
            }

        // Create materials
        PhysicsMaterialList materials;
        for (uint i = 0; i <= max_material_index; ++i)
            materials.push_back(
                    new PhysicsMaterialSimple("Material " + ConvertToString(i), JPH::Color::sGetDistinctColor(i)));

        // Floor
        Body& floor = *body_interface.CreateBody(
                BodyCreationSettings(new MeshShapeSettings(triangles, materials), Vec3::sZero(),
                                     Quat::sRotation(Vec3::sAxisX(), 0.25f * JPH_PI), EMotionType::Static,
                                     PhysicsManager::Layers::NON_MOVING));
        body_interface.AddBody(floor.GetID(), EActivation::DontActivate);

        // 1 body with zero friction to test active edge detection
        Ref<BoxShape> box_shape = new BoxShape(Vec3(2.0f, 2.0f, 2.0f), cDefaultConvexRadius,
                                               new PhysicsMaterialSimple("Box Material", JPH::Color::sYellow));
        Body& body = *body_interface.CreateBody(
                BodyCreationSettings(box_shape, Vec3(0, 55.0f, -50.0f), Quat::sRotation(Vec3::sAxisX(), 0.25f * JPH_PI),
                                     EMotionType::Dynamic, PhysicsManager::Layers::MOVING));
        body.SetFriction(0.0f);
        body_interface.AddBody(body.GetID(), EActivation::Activate);
    }
    scene->play();
}

}  // namespace vox
