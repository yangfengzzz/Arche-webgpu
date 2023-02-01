//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physx/api/mass_local_offset_app.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.render/material/blinn_phong_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"
#include "vox.render/physx/physx_dynamic_collider.h"
#include "vox.render/physx/physx_static_collider.h"
#include "vox.toolkit/controls/orbit_control.h"

namespace vox {
using namespace physx;
namespace {
class PhysicsScript : public Script {
public:
    PhysxDynamicCollider* collider{nullptr};
    PxMaterial* native_material_{nullptr};
    std::shared_ptr<PxBoxGeometry> native_geometry_{nullptr};
    PxShape* native_shape_{nullptr};

    explicit PhysicsScript(Entity* entity) : Script(entity) {
        native_material_ = PhysxManager::GetSingleton().physics->createMaterial(0, 0, 0);
        native_geometry_ = std::make_shared<PxBoxGeometry>(1.5f, 0.5f, 1.0f);
        native_shape_ = PhysxManager::GetSingleton().physics->createShape(*native_geometry_, *native_material_, true);

        collider = entity->addComponent<PhysxDynamicCollider>();
        collider->native_actor_->attachShape(*native_shape_);
        collider->native_actor_->setGlobalPose(PxTransform(PxVec3(0.0f, 0.5f, 0.0f)));
        static_cast<PxRigidDynamic*>(collider->native_actor_)->setMass(1);
        PxRigidBodyExt::setMassAndUpdateInertia(*static_cast<PxRigidDynamic*>(collider->native_actor_), 1);
        static_cast<PxRigidDynamic*>(collider->native_actor_)->setCMassLocalPose(PxTransform(PxVec3(0.0f, 0.0f, 1.5f)));
    }

    void onPhysicsUpdate() override {}
};

}  // namespace

void MassLocalOffsetApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    scene->ambientLight()->setDiffuseSolidColor(Color(1, 1, 1));
    auto rootEntity = scene->createRootEntity();

    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    cameraEntity->addComponent<control::OrbitControl>();

    // floor
    {
        float gDefaultEnvSize = 400;
        float gDefaultEnvThickness = 0.1;
        auto floor_material = PhysxManager::GetSingleton().physics->createMaterial(0, 0, 0);
        auto floor_geometry = std::make_shared<PxBoxGeometry>(gDefaultEnvSize, gDefaultEnvThickness, gDefaultEnvSize);
        auto floor_shape = PhysxManager::GetSingleton().physics->createShape(*floor_geometry, *floor_material, true);

        auto floor = rootEntity->addComponent<PhysxStaticCollider>();
        floor->native_actor_->attachShape(*floor_shape);
        floor->native_actor_->setGlobalPose(PxTransform(PxVec3(0.0f, -gDefaultEnvThickness, 0.0f)));
    }

    // init point light
    auto light = rootEntity->createChild("light");
    light->transform->setPosition(0, 3, 0);
    auto pointLight = light->addComponent<PointLight>();
    pointLight->intensity = 0.3;

    scene->play();

    auto cubeEntity = rootEntity->createChild();
    cubeEntity->addComponent<PhysicsScript>();
    auto renderer = cubeEntity->addComponent<MeshRenderer>();
    renderer->setMesh(PrimitiveMesh::createCuboid(_device, 1.5f, 0.5f, 1.0f));
    auto material = std::make_shared<BlinnPhongMaterial>(_device);
    material->setBaseColor(Color(0.4, 0.6, 0.6));
    renderer->setMaterial(material);
}

}  // namespace vox
