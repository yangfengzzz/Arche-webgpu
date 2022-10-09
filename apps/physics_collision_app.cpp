//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/physics_collision_app.h"

#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include <random>

#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.render/material/blinn_phong_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"
#include "vox.render/physics/collider.h"
#include "vox.toolkit/controls/orbit_control.h"

namespace vox {
namespace {
class MoveScript : public Script {
    float pos_ = -5;
    float vel_ = 0.05;
    int8_t vel_sign_ = -1;

public:
    explicit MoveScript(Entity *entity) : Script(entity) {}

    void onPhysicsUpdate() override {
        if (pos_ >= 5) {
            vel_sign_ = -1;
        }
        if (pos_ <= -5) {
            vel_sign_ = 1;
        }
        pos_ += vel_ * float(vel_sign_);

        entity()->transform->setWorldPosition(pos_, 0, 0);
    }
};

// Collision Detection
class CollisionScript : public Script {
    MeshRenderer *sphere_renderer_;
    std::default_random_engine e_;
    std::uniform_real_distribution<float> u_;

public:
    explicit CollisionScript(Entity *entity) : Script(entity) {
        sphere_renderer_ = entity->getComponent<MeshRenderer>();
        u_ = std::uniform_real_distribution<float>(0, 1);
    }

    void onContactEnter(const Collider &other, const JPH::ContactManifold &inManifold) override {
        static_cast<BlinnPhongMaterial *>(sphere_renderer_->getMaterial().get())
                ->setBaseColor(Color(u_(e_), u_(e_), u_(e_), 1));
    }

    void onContactExit(const Collider &other, const JPH::SubShapeID &shapeID) override {
        static_cast<BlinnPhongMaterial *>(sphere_renderer_->getMaterial().get())
                ->setBaseColor(Color(u_(e_), u_(e_), u_(e_), 1));
    }
};
}  // namespace

void PhysicsCollisionApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    scene->ambientLight()->setDiffuseSolidColor(Color(1, 1, 1));

    auto root_entity = scene->createRootEntity();
    auto camera_entity = root_entity->createChild("camera");
    camera_entity->transform->setPosition(10, 10, 10);
    camera_entity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = camera_entity->addComponent<Camera>();
    camera_entity->addComponent<control::OrbitControl>();

    // init point light
    auto light = root_entity->createChild("light");
    light->transform->setPosition(0, 3, 0);
    auto point_light = light->addComponent<PointLight>();
    point_light->intensity = 0.3;

    // create box test entity
    float cube_size = 2.0;
    auto box_entity = root_entity->createChild("BoxEntity");
    auto box_mtl = std::make_shared<BlinnPhongMaterial>(_device);
    auto box_renderer = box_entity->addComponent<MeshRenderer>();
    box_mtl->setBaseColor(Color(0.8, 0.3, 0.3, 1.0));
    box_renderer->setMesh(PrimitiveMesh::createCuboid(_device, cube_size, cube_size, cube_size));
    box_renderer->setMaterial(box_mtl);

    auto box_collider = box_entity->addComponent<Collider>();
    // boxCollider->debugEntity = boxEntity;
    auto box_collider_shape = std::make_unique<JPH::BoxShape>(JPH::Vec3{cube_size, cube_size, cube_size});
    box_collider->setShape(std::move(box_collider_shape), JPH::EMotionType::Static);

    // create sphere test entity
    float radius = 1.25;
    auto sphere_entity = root_entity->createChild("SphereEntity");
    sphere_entity->transform->setPosition(Point3F(-5, 0, 0));
    auto sphere_renderer = sphere_entity->addComponent<MeshRenderer>();
    auto sphere_mtl = std::make_shared<BlinnPhongMaterial>(_device);
    std::default_random_engine e;
    std::uniform_real_distribution<float> u(0, 1);
    sphere_mtl->setBaseColor(Color(u(e), u(e), u(e), 1));
    sphere_renderer->setMesh(PrimitiveMesh::createSphere(_device, radius));
    sphere_renderer->setMaterial(sphere_mtl);

    auto sphere_collider = sphere_entity->addComponent<Collider>();
    // sphereCollider->debugEntity = sphereEntity;
    auto sphere_collider_shape = std::make_unique<JPH::SphereShape>(radius);
    sphere_collider->setShape(std::move(sphere_collider_shape), JPH::EMotionType::Kinematic);

    sphere_entity->addComponent<CollisionScript>();
    sphere_entity->addComponent<MoveScript>();

    scene->play();
}

}  // namespace vox
