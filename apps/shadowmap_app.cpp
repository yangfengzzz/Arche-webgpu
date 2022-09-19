//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/shadowmap_app.h"

#include "vox.render/camera.h"
#include "vox.render/controls/orbit_control.h"
#include "vox.render/entity.h"
#include "vox.render/material/blinn_phong_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"

namespace vox {
namespace {
class lightMovement : public Script {
public:
    const float speed = 1;
    float totalTime = 0;

    explicit lightMovement(Entity *entity) : Script(entity) {}

    void onUpdate(float deltaTime) override {
        totalTime += deltaTime;
        totalTime = fmod(totalTime, 100);
        entity()->transform->setPosition(10 * std::sin(speed * totalTime), 10, 10 * std::cos(speed * totalTime));
        entity()->transform->lookAt(Point3F(0, 0, 0));
    }
};

class lightMovementReverse : public Script {
public:
    const float speed = 1;
    float totalTime = 0;

    explicit lightMovementReverse(Entity *entity) : Script(entity) {}

    void onUpdate(float deltaTime) override {
        totalTime += deltaTime;
        totalTime = fmod(totalTime, 100);
        entity()->transform->setPosition(10 * std::cos(speed * totalTime), 10, 10 * std::sin(speed * totalTime));
        entity()->transform->lookAt(Point3F(0, 0, 0));
    }
};
}  // namespace

void ShadowMapApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    auto rootEntity = scene->createRootEntity();

    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    cameraEntity->addComponent<control::OrbitControl>();

    // init point light
    auto light = rootEntity->createChild("light");
    light->addComponent<lightMovement>();
    auto spotLight = light->addComponent<SpotLight>();
    spotLight->intensity = 0.2;
    spotLight->enableShadow = true;

    auto light2 = rootEntity->createChild("light2");
    light2->transform->setPosition(0, 10, 0);
    light2->transform->lookAt(Point3F(0, 0, 0), Vector3F(1, 0, 0));
    auto pointLight = light2->addComponent<PointLight>();
    pointLight->intensity = 0.2;
    pointLight->enableShadow = true;  // slow

    auto light3 = rootEntity->createChild("light3");
    light3->addComponent<lightMovementReverse>();
    auto directLight = light3->addComponent<DirectLight>();
    directLight->intensity = 0.2;
    directLight->enableShadow = true;

    // create box test entity
    float cubeSize = 2.0;
    auto boxEntity = rootEntity->createChild("BoxEntity");
    boxEntity->transform->setPosition(Point3F(0, 2, 0));
    auto boxMtl = std::make_shared<BlinnPhongMaterial>(_device);
    boxMtl->setBaseColor(Color(0.3, 0.3, 0.3, 0.5));

    auto boxRenderer = boxEntity->addComponent<MeshRenderer>();
    boxRenderer->setMesh(PrimitiveMesh::createCuboid(_device, cubeSize, cubeSize, cubeSize));
    boxRenderer->setMaterial(boxMtl);
    boxRenderer->castShadow = true;

    auto planeEntity = rootEntity->createChild("PlaneEntity");
    auto planeMtl = std::make_shared<BlinnPhongMaterial>(_device);
    planeMtl->setBaseColor(Color(1.0, 0, 0, 1.0));
    planeMtl->setRenderFace(RenderFace::Double);

    auto planeRenderer = planeEntity->addComponent<MeshRenderer>();
    planeRenderer->setMesh(PrimitiveMesh::createPlane(_device, 10, 10));
    planeRenderer->setMaterial(planeMtl);
    planeRenderer->setReceiveShadows(true);

    scene->play();
}

}  // namespace vox
