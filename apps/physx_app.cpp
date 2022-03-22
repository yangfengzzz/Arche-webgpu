//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "physx_app.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/unlit_material.h"
#include "material/blinn_phong_material.h"
#include "camera.h"
#include "physics/static_collider.h"
#include "physics/dynamic_collider.h"
#include "physics/shape/box_collider_shape.h"
#include "physics/shape/sphere_collider_shape.h"
#include "lighting/point_light.h"
#include "controls/orbit_control.h"
#include <random>

namespace vox {
namespace {
class MoveScript : public Script {
    Point3F pos = Point3F(-5, 0, 0);
    float vel = 4;
    int8_t velSign = -1;
    
public:
    MoveScript(Entity *entity) : Script(entity) {
    }
    
    void onUpdate(float deltaTime) override {
        if (pos.x >= 5) {
            velSign = -1;
        }
        if (pos.x <= -5) {
            velSign = 1;
        }
        pos.x += deltaTime * vel * float(velSign);
        
        entity()->transform->setPosition(pos);
    }
};

// Collision Detection
class CollisionScript : public Script {
    MeshRenderer *sphereRenderer;
    std::default_random_engine e;
    std::uniform_real_distribution<float> u;
    
public:
    CollisionScript(Entity *entity) : Script(entity) {
        sphereRenderer = entity->getComponent<MeshRenderer>();
        u = std::uniform_real_distribution<float>(0, 1);
    }
    
    void onTriggerExit(physics::ColliderShapePtr other) override {
        static_cast<BlinnPhongMaterial *>(sphereRenderer->getMaterial().get())->setBaseColor(Color(u(e), u(e), u(e), 1));
    }
    
    void onTriggerEnter(physics::ColliderShapePtr other) override {
        static_cast<BlinnPhongMaterial *>(sphereRenderer->getMaterial().get())->setBaseColor(Color(u(e), u(e), u(e), 1));
    }
};
} // namespace

void PhysXApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    scene->ambientLight().setDiffuseSolidColor(Color(1, 1, 1));
    
    auto rootEntity = scene->createRootEntity();
    auto cameraEntity = rootEntity->createChild("camera");
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    cameraEntity->addComponent<control::OrbitControl>();
    
    // init point light
    auto light = rootEntity->createChild("light");
    light->transform->setPosition(0, 3, 0);
    auto pointLight = light->addComponent<PointLight>();
    pointLight->intensity = 0.3;
    
    // create box test entity
    float cubeSize = 2.0;
    auto boxEntity = rootEntity->createChild("BoxEntity");
    auto boxMtl = std::make_shared<BlinnPhongMaterial>(_device);
    auto boxRenderer = boxEntity->addComponent<MeshRenderer>();
    boxMtl->setBaseColor(Color(0.8, 0.3, 0.3, 1.0));
    boxRenderer->setMesh(PrimitiveMesh::createCuboid(_device, cubeSize, cubeSize, cubeSize));
    boxRenderer->setMaterial(boxMtl);
    
    auto boxCollider = boxEntity->addComponent<physics::StaticCollider>();
    // boxCollider->debugEntity = boxEntity;
    auto boxColliderShape = std::make_shared<physics::BoxColliderShape>();
    boxColliderShape->setSize(Vector3F(cubeSize, cubeSize, cubeSize));
    boxCollider->addShape(boxColliderShape);
    
    // create sphere test entity
    float radius = 1.25;
    auto sphereEntity = rootEntity->createChild("SphereEntity");
    sphereEntity->transform->setPosition(Point3F(-5, 0, 0));
    auto sphereRenderer = sphereEntity->addComponent<MeshRenderer>();
    auto sphereMtl = std::make_shared<BlinnPhongMaterial>(_device);
    std::default_random_engine e;
    std::uniform_real_distribution<float> u(0, 1);
    sphereMtl->setBaseColor(Color(u(e), u(e), u(e), 1));
    sphereRenderer->setMesh(PrimitiveMesh::createSphere(_device, radius));
    sphereRenderer->setMaterial(sphereMtl);
    
    auto sphereCollider = sphereEntity->addComponent<physics::DynamicCollider>();
    // sphereCollider->debugEntity = sphereEntity;
    auto sphereColliderShape = std::make_shared<physics::SphereColliderShape>();
    sphereColliderShape->setRadius(radius);
    sphereColliderShape->setTrigger(true);
    sphereCollider->addShape(sphereColliderShape);
    
    sphereEntity->addComponent<CollisionScript>();
    sphereEntity->addComponent<MoveScript>();
}

}
