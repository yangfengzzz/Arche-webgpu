//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "Irradiance_app.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/pbr_material.h"
#include "camera.h"
#include "image/stb.h"
#include "shaderlib/wgsl_skybox_debugger.h"

namespace vox {
class BakerMaterial : public BaseMaterial {
public:
    BakerMaterial(wgpu::Device& device) : BaseMaterial(device, Shader::find("cubemapDebugger")) {
    }
    
    /// Base texture.
    SampledTexture2DPtr baseTexture() {
        return _texture;
    }
    
    void setBaseTexture(SampledTexture2DPtr newValue) {
        _texture = newValue;
        shaderData.setSampledTexture(_baseTextureProp, _baseSamplerProp, newValue);
    }
    
    /// Tiling and offset of main textures.
    uint32_t faceIndex() {
        return _faceIndex;
    }
    
    void setFaceInex(uint32_t newValue) {
        _faceIndex = newValue;
        shaderData.setData(_faceIndexProp, newValue);
    }
    
private:
    SampledTexture2DPtr _texture{nullptr};
    ShaderProperty _baseTextureProp = Shader::createProperty("u_baseTexture", ShaderDataGroup::Material);
    ShaderProperty _baseSamplerProp = Shader::createProperty("u_baseSampler", ShaderDataGroup::Material);
    
    uint32_t _faceIndex;
    ShaderProperty _faceIndexProp = Shader::createProperty("u_faceIndex", ShaderDataGroup::Material);
};

void IrradianceApp::loadScene(uint32_t width, uint32_t height) {
    auto rootEntity = _scene->createRootEntity();
    Shader::create("cubemapDebugger", std::make_unique<WGSLSkyboxDebuggerVertex>(), std::make_unique<WGSLSkyboxDebuggerFragment>());
    
    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(0, 0, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    _mainCamera->resize(width, height);
    cameraEntity->addComponent<control::OrbitControl>();
    
    // Create Sphere
    auto sphereEntity = rootEntity->createChild("box");
    sphereEntity->transform->setPosition(-1, 2, 0);
    auto sphereMaterial = std::make_shared<PBRMaterial>(_device);
    sphereMaterial->setRoughness(0);
    sphereMaterial->setMetallic(1);
    auto renderer = sphereEntity->addComponent<MeshRenderer>();
    renderer->setMesh(PrimitiveMesh::createSphere(_device, 1, 64));
    renderer->setMaterial(sphereMaterial);
    
    // Create planes
    std::array<EntityPtr, 6> planes{};
    std::array<std::shared_ptr<BakerMaterial>, 6> planeMaterials{};
    
    for (int i = 0; i < 6; i++) {
        auto bakerEntity = rootEntity->createChild("IBL Baker Entity");
        bakerEntity->transform->setRotation(90, 0, 0);
        auto bakerMaterial = std::make_shared<BakerMaterial>(_device);
        auto bakerRenderer = bakerEntity->addComponent<MeshRenderer>();
        bakerRenderer->setMesh(PrimitiveMesh::createPlane(_device, 2, 2));
        bakerRenderer->setMaterial(bakerMaterial);
        planes[i] = bakerEntity;
        planeMaterials[i] = bakerMaterial;
    }
    
    planes[0]->transform->setPosition(1, 0, 0); // PX
    planes[1]->transform->setPosition(-3, 0, 0); // NX
    planes[2]->transform->setPosition(1, 2, 0); // PY
    planes[3]->transform->setPosition(1, -2, 0); // NY
    planes[4]->transform->setPosition(-1, 0, 0); // PZ
    planes[5]->transform->setPosition(3, 0, 0); // NZ
    
    const std::string path = "SkyMap/country/";
    const std::array<std::string, 6> imageNames = {"posx.png", "negx.png", "posy.png", "negy.png", "posz.png", "negz.png"};
    std::array<std::unique_ptr<Image>, 6> images;
    std::array<Image*, 6> imagePtr;
    for (int i = 0; i < 6; i++) {
        images[i] = Image::load(path + imageNames[i]);
        imagePtr[i] = images[i].get();
    }
    _cubeMap = std::make_shared<SampledTextureCube>(_device,
                                                    images[0]->extent().width, images[0]->extent().height, 1,
                                                    images[0]->format());
    _cubeMap->setPixelBuffer(imagePtr);
    _scene->ambientLight().setSpecularTexture(_cubeMap);
    
    auto changeMip = [&](uint32_t mipLevel) {
        for (uint32_t i = 0; i < 6; i++) {
            auto material = planeMaterials[i];
            material->setBaseTexture(_cubeMap->textureView2D(mipLevel, i));
            material->setFaceInex(i);
        }
    };
    changeMip(0);
}

}
