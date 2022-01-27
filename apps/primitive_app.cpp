//
//  primitive_app.cpp
//  apps
//
//  Created by 杨丰 on 2022/1/20.
//

#include "primitive_app.h"
#include "entity.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/unlit_material.h"
#include "camera.h"
#include "controls/orbit_control.h"

namespace vox {
void PrimitiveApp::loadScene(uint32_t width, uint32_t height) {
    auto rootEntity = _scene->createRootEntity();
    auto modelEntity = rootEntity->createChild();
    
    auto renderer = modelEntity->addComponent<MeshRenderer>();
    renderer->setMesh(PrimitiveMesh::createCuboid(_device));
    auto material = std::make_shared<UnlitMaterial>(_device);
    material->setBaseColor(Color(0.6, 0.4, 0.7, 1.0));
    renderer->setMaterial(material);
    
    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    _mainCamera->resize(width, height);
    cameraEntity->addComponent<control::OrbitControl>();
}

}