//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shadowmap_app.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/blinn_phong_material.h"
#include "controls/orbit_control.h"
#include "camera.h"
#include "lighting/direct_light.h"
#include "lighting/spot_light.h"

namespace vox {
namespace {
class lightMovemenet : public Script {
public:
    const float speed = 1;
    float totalTime = 0;
    
    lightMovemenet(Entity *entity) : Script(entity) {
    }
    
    void onUpdate(float deltaTime) override {
        totalTime += deltaTime;
        totalTime = fmod(totalTime, 100);
        entity()->transform->setPosition(10 * std::sin(speed * totalTime), 10, 10 * std::cos(speed * totalTime));
        entity()->transform->lookAt(Point3F(0, 0, 0));
    }
};

class lightMovemenetReverse : public Script {
public:
    const float speed = 1;
    float totalTime = 0;
    
    lightMovemenetReverse(Entity *entity) : Script(entity) {
    }
    
    void onUpdate(float deltaTime) override {
        totalTime += deltaTime;
        totalTime = fmod(totalTime, 100);
        entity()->transform->setPosition(10 * std::cos(speed * totalTime), 10, 10 * std::sin(speed * totalTime));
        entity()->transform->lookAt(Point3F(0, 0, 0));
    }
};
}

void ShadowMapApp::loadScene(uint32_t width, uint32_t height) {
    auto rootEntity = _scene->createRootEntity();
    
    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    _mainCamera->resize(width, height);
    cameraEntity->addComponent<control::OrbitControl>();
    
    // init point light
    auto light = rootEntity->createChild("light");
    light->addComponent<lightMovemenet>();
    auto spotLight = light->addComponent<SpotLight>();
    spotLight->intensity = 0.2;
    spotLight->setEnableShadow(true);
    
    auto light2 = rootEntity->createChild("light2");
    light2->transform->setPosition(0, 10, 0);
    light2->transform->lookAt(Point3F(0, 0, 0), Vector3F(1, 0, 0));
    auto pointLight = light2->addComponent<PointLight>();
    pointLight->intensity = 0.2;
    pointLight->setEnableShadow(true); // slow
    
    auto light3 = rootEntity->createChild("light3");
    light3->addComponent<lightMovemenetReverse>();
    auto directLight = light3->addComponent<DirectLight>();
    directLight->intensity = 0.2;
    directLight->setEnableShadow(true);
    
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
    planeRenderer->receiveShadow = true;
}

}
