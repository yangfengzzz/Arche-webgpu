//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/cluster_forward_app.h"

#include <random>

#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.render/lighting/debug/cluster_debug_material.h"
#include "vox.render/lighting/debug/sprite_debug.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"
#include "vox.toolkit/controls/orbit_control.h"

namespace vox {
void ClusterForwardApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    scene->ambientLight()->setDiffuseSolidColor(Color(1, 1, 1));

    auto rootEntity = scene->createRootEntity();
    rootEntity->addComponent<SpriteDebug>();

    auto cameraEntity = rootEntity->createChild("camera");
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    cameraEntity->addComponent<control::OrbitControl>();

    // init point light
    auto light = rootEntity->createChild("light");
    light->transform->setPosition(3, 3, 0);
    light->addComponent<PointLight>();

    auto light2 = rootEntity->createChild("light");
    light2->transform->setPosition(-3, 3, 0);
    light2->addComponent<SpotLight>();

    // create box test entity
    float cubeSize = 20.0;
    auto boxEntity = rootEntity->createChild("BoxEntity");
    auto boxMtl = std::make_shared<ClusterDebugMaterial>(_device);
    auto boxRenderer = boxEntity->addComponent<MeshRenderer>();
    boxRenderer->setMesh(PrimitiveMesh::createPlane(_device, cubeSize, cubeSize));
    boxRenderer->setMaterial(boxMtl);

    scene->play();
}

}  // namespace vox
