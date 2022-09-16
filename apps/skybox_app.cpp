//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/skybox_app.h"

#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.render/material/unlit_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"
#include "vox.render/rendering/subpasses/skybox_subpass.h"

namespace vox {
bool SkyboxApp::prepare(Platform &platform) {
    ForwardApplication::prepare(platform);

    auto scene = _sceneManager->currentScene();
    auto skybox = std::make_unique<SkyboxSubpass>(_renderContext.get(), _depthStencilTextureFormat, scene, _mainCamera);
    skybox->createCuboid();
    skybox->setTextureCubeMap(ImageManager::GetSingleton().loadTexture("Textures/uffizi_rgba16f_cube.ktx"));
    _renderPass->addSubpass(std::move(skybox));

    return true;
}

void SkyboxApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    auto rootEntity = scene->createRootEntity();

    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    cameraEntity->addComponent<control::OrbitControl>();

    auto modelEntity = rootEntity->createChild();
    auto renderer = modelEntity->addComponent<MeshRenderer>();
    renderer->setMesh(PrimitiveMesh::createCuboid(_device));
    auto material = std::make_shared<UnlitMaterial>(_device);
    material->setBaseColor(Color(0.6, 0.4, 0.7, 1.0));
    renderer->setMaterial(material);

    scene->play();
}

}  // namespace vox
