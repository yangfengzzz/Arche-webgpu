//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/ibl_app.h"

#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.render/material/pbr_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"
#include "vox.render/rendering/subpasses/skybox_subpass.h"

namespace vox {
bool IBLApp::prepare(Platform &platform) {
    ForwardApplication::prepare(platform);

    auto scene = _sceneManager->currentScene();
    auto skybox = std::make_unique<SkyboxSubpass>(_renderContext.get(), _depthStencilTextureFormat, scene, _mainCamera);
    skybox->createCuboid();
    skybox->setTextureCubeMap(_cubeMap);
    _renderPass->addSubpass(std::move(skybox));

    return true;
}

void IBLApp::loadScene() {
    _materials.resize(11);
    _materials[0] = Material("Gold", Color(1.0f, 0.765557f, 0.336057f, 1.0), 0.1f, 1.0f);
    _materials[1] = Material("Copper", Color(0.955008f, 0.637427f, 0.538163f, 1.0), 0.1f, 1.0f);
    _materials[2] = Material("Chromium", Color(0.549585f, 0.556114f, 0.554256f, 1.0), 0.1f, 1.0f);
    _materials[3] = Material("Nickel", Color(0.659777f, 0.608679f, 0.525649f, 1.0), 0.1f, 1.0f);
    _materials[4] = Material("Titanium", Color(0.541931f, 0.496791f, 0.449419f, 1.0), 0.1f, 1.0f);
    _materials[5] = Material("Cobalt", Color(0.662124f, 0.654864f, 0.633732f, 1.0), 0.1f, 1.0f);
    _materials[6] = Material("Platinum", Color(0.672411f, 0.637331f, 0.585456f, 1.0), 0.1f, 1.0f);
    // Testing materials
    _materials[7] = Material("White", Color(1.0f, 1.0, 1.0, 1.0), 0.1f, 1.0f);
    _materials[8] = Material("Red", Color(1.0f, 0.0f, 0.0f, 1.0), 0.1f, 1.0f);
    _materials[9] = Material("Blue", Color(0.0f, 0.0f, 1.0f, 1.0), 0.1f, 1.0f);
    _materials[10] = Material("Black", Color(0.0f, 1.0, 1.0, 1.0), 0.1f, 1.0f);

    const int materialIndex = 7;
    Material mat = _materials[materialIndex];

    const std::string path = "SkyMap/country/";
    const std::array<std::string, 6> imageNames = {"posx.png", "negx.png", "posy.png",
                                                   "negy.png", "posz.png", "negz.png"};
    std::array<std::unique_ptr<Image>, 6> images;
    std::array<Image *, 6> imagePtr{};
    for (int i = 0; i < 6; i++) {
        images[i] = Image::load(path + imageNames[i]);
        imagePtr[i] = images[i].get();
    }
    _cubeMap = std::make_shared<SampledTextureCube>(_device, images[0]->extent().width, images[0]->extent().height, 1,
                                                    images[0]->format());
    _cubeMap->setPixelBuffer(imagePtr);

    auto scene = _sceneManager->currentScene();
    scene->ambientLight()->setSpecularTexture(_cubeMap);
    scene->ambientLight()->setDiffuseTexture(_cubeMap);

    auto rootEntity = scene->createRootEntity();
    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    cameraEntity->addComponent<control::OrbitControl>();

    auto light = rootEntity->createChild("light");
    light->transform->setPosition(-3, 3, -3);
    light->transform->lookAt(Point3F(0, 0, 0));
    auto directionLight = light->addComponent<DirectLight>();
    directionLight->intensity = 0.3;

    auto sphere = PrimitiveMesh::createSphere(_device, 0.5, 64);
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            auto sphereEntity = rootEntity->createChild("SphereEntity" + std::to_string(i) + std::to_string(j));
            sphereEntity->transform->setPosition(Point3F(i - 3, j - 3, 0));
            auto sphereMtl = std::make_shared<PBRMaterial>(_device);
            sphereMtl->setBaseColor(mat.baseColor);
            sphereMtl->setMetallic(clamp(float(7 - i) / float(7 - 1), 0.1f, 1.0f));
            sphereMtl->setRoughness(clamp(float(7 - j) / float(7 - 1), 0.05f, 1.0f));

            auto sphereRenderer = sphereEntity->addComponent<MeshRenderer>();
            sphereRenderer->setMesh(sphere);
            sphereRenderer->setMaterial(sphereMtl);
        }
    }

    scene->play();
}

}  // namespace vox
