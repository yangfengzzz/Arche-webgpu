//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cluster_forward_app.h"
#include "mesh/buffer_mesh.h"
#include "mesh/mesh_renderer.h"
#include "camera.h"
#include "controls/orbit_control.h"
#include "lighting/point_light.h"
#include "lighting/cluster_debug_material.h"
#include <random>

namespace vox {
void ClusterForwardApp::loadScene(uint32_t width, uint32_t height) {
    _scene->ambientLight().setDiffuseSolidColor(Color(1, 1, 1));
    
    auto rootEntity = _scene->createRootEntity();
    auto cameraEntity = rootEntity->createChild("camera");
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    _mainCamera->resize(width, height);
    cameraEntity->addComponent<control::OrbitControl>();
    
    // init point light
    auto light = rootEntity->createChild("light");
    light->transform->setPosition(0, 3, 0);
    auto pointLight = light->addComponent<PointLight>();
    pointLight->intensity = 0.3;
    
    // create box test entity
    auto boxEntity = rootEntity->createChild("BoxEntity");
    auto boxMtl = std::make_shared<ClusterDebugMaterial>(_device);
    auto boxRenderer = boxEntity->addComponent<MeshRenderer>();
    
    auto mesh = std::make_shared<BufferMesh>();
    mesh->addSubMesh(0, 6);
    boxRenderer->setMesh(mesh);
    boxRenderer->setMaterial(boxMtl);
}

}
