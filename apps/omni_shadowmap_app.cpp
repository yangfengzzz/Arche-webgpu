//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/omni_shadowmap_app.h"

#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.render/material/blinn_phong_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"

namespace vox {
namespace {
class MoveScript : public Script {
    float totalTime = 0;
    float height = 2;
    float vel = 4;
    int8_t velSign = -1;

public:
    explicit MoveScript(Entity *entity) : Script(entity) {}

    void onUpdate(float deltaTime) override {
        if (height >= 2) {
            velSign = -1;
        }
        if (height <= -2) {
            velSign = 1;
        }
        height += deltaTime * vel * float(velSign);

        entity()->transform->setPosition(std::sin(totalTime), height, std::cos(totalTime));
        totalTime += deltaTime;
    }
};
}  // namespace

void OminiShadowMapApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    auto rootEntity = scene->createRootEntity();

    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(0, 0, 20);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    cameraEntity->addComponent<control::OrbitControl>();

    auto light = rootEntity->createChild("light");
    light->transform->setPosition(0, 0, 0);
    auto directLight = light->addComponent<PointLight>();
    directLight->intensity = 0.5;
    directLight->setEnableShadow(true);

    auto planeMesh = PrimitiveMesh::createPlane(_device, 10, 10);

    auto planeEntity = rootEntity->createChild("PlaneEntity");
    planeEntity->transform->setPosition(0, 5, 0);
    auto planeMtl = std::make_shared<BlinnPhongMaterial>(_device);
    planeMtl->setBaseColor(Color(0.2, 0.2, 0.2, 1.0));
    planeMtl->setRenderFace(RenderFace::Double);
    auto planeRenderer = planeEntity->addComponent<MeshRenderer>();
    planeRenderer->setMesh(planeMesh);
    planeRenderer->setMaterial(planeMtl);
    planeRenderer->receiveShadow = true;

    auto planeEntity2 = rootEntity->createChild("PlaneEntity2");
    planeEntity2->transform->setPosition(0, -5, 0);
    auto planeMtl2 = std::make_shared<BlinnPhongMaterial>(_device);
    planeMtl2->setBaseColor(Color(0.4, 0.4, 0.4, 1.0));
    planeMtl2->setRenderFace(RenderFace::Double);
    auto planeRenderer2 = planeEntity2->addComponent<MeshRenderer>();
    planeRenderer2->setMesh(planeMesh);
    planeRenderer2->setMaterial(planeMtl2);
    planeRenderer2->receiveShadow = true;

    auto planeEntity3 = rootEntity->createChild("PlaneEntity3");
    planeEntity3->transform->setPosition(5, 0, 0);
    planeEntity3->transform->setRotation(0, 0, 90);
    auto planeMtl3 = std::make_shared<BlinnPhongMaterial>(_device);
    planeMtl3->setBaseColor(Color(0.6, 0.6, 0.6, 1.0));
    planeMtl3->setRenderFace(RenderFace::Double);
    auto planeRenderer3 = planeEntity3->addComponent<MeshRenderer>();
    planeRenderer3->setMesh(planeMesh);
    planeRenderer3->setMaterial(planeMtl3);
    planeRenderer3->receiveShadow = true;

    auto planeEntity4 = rootEntity->createChild("PlaneEntity4");
    planeEntity4->transform->setPosition(-5, 0, 0);
    planeEntity4->transform->setRotation(0, 0, -90);
    auto planeMtl4 = std::make_shared<BlinnPhongMaterial>(_device);
    planeMtl4->setBaseColor(Color(0.8, 0.8, 0.8, 1.0));
    planeMtl4->setRenderFace(RenderFace::Double);
    auto planeRenderer4 = planeEntity4->addComponent<MeshRenderer>();
    planeRenderer4->setMesh(planeMesh);
    planeRenderer4->setMaterial(planeMtl4);
    planeRenderer4->receiveShadow = true;

    auto planeEntity5 = rootEntity->createChild("PlaneEntity5");
    planeEntity5->transform->setPosition(0, 0, -5);
    planeEntity5->transform->setRotation(90, 0, 0);
    auto planeMtl5 = std::make_shared<BlinnPhongMaterial>(_device);
    planeMtl5->setBaseColor(Color(1.0, 1.0, 1.0, 1.0));
    planeMtl5->setRenderFace(RenderFace::Double);
    auto planeRenderer5 = planeEntity5->addComponent<MeshRenderer>();
    planeRenderer5->setMesh(planeMesh);
    planeRenderer5->setMaterial(planeMtl5);
    planeRenderer5->receiveShadow = true;

    // create box test entity
    float cubeSize = 1.0;
    auto boxMesh = PrimitiveMesh::createCuboid(_device, cubeSize, cubeSize, cubeSize);
    auto boxMtl = std::make_shared<BlinnPhongMaterial>(_device);
    boxMtl->setBaseColor(Color(1.0, 0.0, 0.0, 0.5));
    boxMtl->setRenderFace(RenderFace::Double);  // bug
    auto boxEntity = rootEntity->createChild("BoxEntity");
    boxEntity->addComponent<MoveScript>();
    auto boxRenderer = boxEntity->addComponent<MeshRenderer>();
    boxRenderer->setMesh(boxMesh);
    boxRenderer->setMaterial(boxMtl);
    boxRenderer->castShadow = true;

    scene->play();
}

}  // namespace vox
