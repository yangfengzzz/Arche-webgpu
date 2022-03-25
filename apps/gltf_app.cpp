//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gltf_app.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "loader/gltf_loader.h"
#include "controls/free_control.h"
#include "camera.h"

namespace vox {
void GLTFApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    auto rootEntity = scene->createRootEntity();
    
    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(5, 5, 1);
    _mainCamera = cameraEntity->addComponent<Camera>();
    cameraEntity->addComponent<control::FreeControl>();
    
    // init point light
    auto light = rootEntity->createChild("light");
    light->transform->setPosition(5.0f, 5.0f, -5.0f);
    light->addComponent<PointLight>();
    
    auto modelEntity = rootEntity->createChild();
    auto loader = loader::GLTFLoader(_device);
    loader.loadFromFile("Models/sponza/sponza.gltf", modelEntity);
    
    scene->play();
}

}
