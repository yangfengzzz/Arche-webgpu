//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/cascade_shadowmap_app.h"

#include "vox.render/camera.h"
#include "vox.render/controls/orbit_control.h"
#include "vox.render/entity.h"
#include "vox.render/material/blinn_phong_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"
#include "vox.render/shadow/shadow_debug_material.h"

namespace vox {
void CascadeShadowMapApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    auto rootEntity = scene->createRootEntity();

    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(0, 10, 50);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    cameraEntity->addComponent<control::OrbitControl>();

    auto light = rootEntity->createChild("light");
    light->transform->setPosition(10, 10, 0);
    light->transform->lookAt(Point3F());
    auto directLight = light->addComponent<DirectLight>();
    directLight->intensity = 1.0;
    directLight->setEnableShadow(true);

    // create box test entity
    float cubeSize = 2.0;
    auto boxMesh = PrimitiveMesh::createCuboid(_device, cubeSize, cubeSize, cubeSize);
    auto boxMtl = std::make_shared<BlinnPhongMaterial>(_device);
    boxMtl->setBaseColor(Color(0.3, 0.3, 0.3, 0.5));
    for (int i = 0; i < 40; i++) {
        auto boxEntity = rootEntity->createChild("BoxEntity");
        boxEntity->transform->setPosition(Point3F(0, 2, i * 10 - 200));

        auto boxRenderer = boxEntity->addComponent<MeshRenderer>();
        boxRenderer->setMesh(boxMesh);
        boxRenderer->setMaterial(boxMtl);
        boxRenderer->castShadow = true;
    }

    auto planeEntity = rootEntity->createChild("PlaneEntity");
    auto planeMtl = std::make_shared<BlinnPhongMaterial>(_device);
    planeMtl->setBaseColor(Color(1.0, 0, 0, 1.0));
    planeMtl->setRenderFace(RenderFace::Double);

    auto shadowDebug = std::make_shared<ShadowDebugMaterial>(_device);

    auto planeRenderer = planeEntity->addComponent<MeshRenderer>();
    planeRenderer->setMesh(PrimitiveMesh::createPlane(_device, 10, 400));
    planeRenderer->setMaterial(planeMtl);
    // planeRenderer->setMaterial(shadowDebug);
    planeRenderer->receiveShadow = true;

    scene->play();
}

}  // namespace vox
