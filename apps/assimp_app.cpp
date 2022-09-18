//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/assimp_app.h"

#include <assimp/postprocess.h>

#include "vox.render/assimp_parser.h"
#include "vox.render/camera.h"
#include "vox.render/controls/orbit_control.h"
#include "vox.render/entity.h"
#include "vox.render/mesh/mesh_renderer.h"

namespace vox {
void AssimpApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    auto root_entity = scene->createRootEntity();

    auto camera_entity = root_entity->createChild();
    camera_entity->transform->setPosition(10, 10, 10);
    camera_entity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = camera_entity->addComponent<Camera>();
    camera_entity->addComponent<control::OrbitControl>();

    // init point light
    auto light = root_entity->createChild("light");
    light->transform->setPosition(0, 3, 0);
    auto point_light = light->addComponent<PointLight>();
    point_light->intensity = 1.0;
    point_light->distance = 100;

    auto cube_entity = root_entity->createChild();
    cube_entity->transform->setScale(0.01, 0.01, 0.01);
    AssimpParser parser(_device);
    parser.loadModel(cube_entity, "Models/Temple.obj", aiProcess_FlipUVs);

    scene->play();
}

}  // namespace vox
