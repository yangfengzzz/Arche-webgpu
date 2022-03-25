//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "kapla_app.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/unlit_material.h"
#include "material/blinn_phong_material.h"
#include "camera.h"
#include "physics/static_collider.h"
#include "physics/dynamic_collider.h"
#include "physics/character_controller/capsule_character_controller.h"
#include "physics/joint/fixed_joint.h"
#include "physics/shape/box_collider_shape.h"
#include "physics/shape/sphere_collider_shape.h"
#include "physics/shape/plane_collider_shape.h"
#include "physics/shape/capsule_collider_shape.h"
#include "lighting/point_light.h"
#include "controls/orbit_control.h"
#include <random>

namespace vox {
int DEFAULT_SOLVER_ITERATIONS = 8;

void KaplaApp::createCylindricalTower(Entity* entity,
                                      uint32_t nbRadialPoints, float maxRadius, float minRadius, uint32_t height,
                                      const Vector3F& dims, const Vector3F& centerPos, physics::PxMaterial *material,
                                      physics::PxFilterData &simFilterData, physics::PxFilterData &queryFilterData,
                                      float density, bool bUseSweeps, bool bStartAsleep) {
    auto box = PrimitiveMesh::createCuboid(_device, dims.x,  dims.y,  dims.z);
    auto boxMtl = std::make_shared<BlinnPhongMaterial>(_device);
    boxMtl->setBaseColor(Color(1, 0.0, 0.0, 1.0));
    float startHeight = 0.f;
    for (uint32_t i = 0; i < height; ++i) {
        float radius = minRadius + (maxRadius - minRadius) * (1.f - float(i) / float(height));
        for (uint32_t a = 0; a < nbRadialPoints; ++a) {
            float angle = 6.28f * float(a) / float(nbRadialPoints);
            Vector3F innerPos(cosf(angle) * radius, dims.y + startHeight, sinf(angle) * radius);
            QuaternionF rot(Vector3F(0.f, 1.f, 0.f), 3.14 / 2.f - angle);
            auto translation = innerPos + centerPos;
            
            auto boxEntity = entity->createChild();
            boxEntity->transform->setPosition(translation.x, translation.y, translation.z);
            boxEntity->transform->setRotationQuaternion(rot);
            auto renderer = boxEntity->addComponent<MeshRenderer>();
            renderer->setMesh(box);
            renderer->setMaterial(boxMtl);
            
            auto innerBox = boxEntity->addComponent<physics::DynamicCollider>();
            innerBox->setSolverIterations(DEFAULT_SOLVER_ITERATIONS);
            innerBox->setMaxDepenetrationVelocity(2.f);
            // PxRigidBodyExt::updateMassAndInertia(*innerBox, density);
            innerBox->setInertiaTensor(innerBox->inertiaTensor() * 4.f);
            
            auto boxColliderShape = std::make_shared<physics::BoxColliderShape>();
            boxColliderShape->setMaterial(material);
            boxColliderShape->setSize(dims);
            innerBox->addShape(boxColliderShape);
            
            if (bStartAsleep)
                innerBox->putToSleep();
        }
        
        startHeight += 4.f * dims.y;
        density *= 0.975f;
    }
}


void KaplaApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    scene->ambientLight()->setDiffuseSolidColor(Color(1, 1, 1));
    
    auto rootEntity = scene->createRootEntity();
    auto cameraEntity = rootEntity->createChild("camera");
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    cameraEntity->addComponent<control::OrbitControl>();
    
    // init point light
    auto light = rootEntity->createChild("light");
    light->transform->setPosition(0, 10, 0);
    auto pointLight = light->addComponent<PointLight>();
    pointLight->distance = 100;
    pointLight->intensity = 0.3;
    
    const Vector3F dims(0.08f, 0.25f, 1.0f);
    physics::PxMaterial *DefaultMaterial = physics::PhysicsManager::_nativePhysics()->createMaterial(0.4f, 0.15f, 0.1f);
    
    const uint32_t nbOuterRadialLayouts = 10;
    const float outerRadius = 4;
    physics::PxFilterData queryFilterData;
    physics::PxFilterData simFilterData;
    createCylindricalTower(rootEntity, nbOuterRadialLayouts, outerRadius, 0, 8, dims, Vector3F(0.f, 0.f, 0.f),
                           DefaultMaterial, simFilterData, queryFilterData);
    
    auto addPlane = [&](const Vector2F &size, const Point3F &position, const QuaternionF &rotation) {
        auto mtl = std::make_shared<BlinnPhongMaterial>(_device);
        mtl->setBaseColor(Color(0.03179807202597362, 0.3939682161541871, 0.41177952549087604, 1.0));
        auto planeEntity = rootEntity->createChild();
        
        auto renderer = planeEntity->addComponent<MeshRenderer>();
        renderer->receiveShadow = true;
        renderer->setMesh(PrimitiveMesh::createPlane(_device, size.x, size.y));
        renderer->setMaterial(mtl);
        planeEntity->transform->setPosition(position);
        planeEntity->transform->setRotationQuaternion(rotation);
        
        auto physicsPlane = std::make_shared<physics::PlaneColliderShape>();
        auto planeCollider = planeEntity->addComponent<physics::StaticCollider>();
        planeCollider->addShape(physicsPlane);
        
        return planeEntity;
    };
    addPlane(Vector2F(30, 30), Point3F(), QuaternionF());

    scene->play();
}

}
