//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/rendering/irradiance_app.h"

#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.render/material/pbr_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"
#include "vox.render/shader/shader_manager.h"

namespace vox {
class BakerMaterial : public BaseMaterial {
public:
    explicit BakerMaterial(wgpu::Device& device) : BaseMaterial(device) {
        vertex_source_ = ShaderManager::GetSingleton().LoadShader("cubemap-debugger.vert");
        fragment_source_ = ShaderManager::GetSingleton().LoadShader("cubemap-debugger.frag");
    }

    /// Base texture.
    void setBaseTexture(const std::shared_ptr<ImageView>& newValue) {
        shaderData.setImageView(_baseTextureProp, _baseSamplerProp, newValue);
    }

    /// Tiling and offset of main textures.
    [[nodiscard]] uint32_t faceIndex() const { return _faceIndex; }

    void setFaceIndex(uint32_t newValue) {
        _faceIndex = newValue;
        shaderData.setData(_faceIndexProp, newValue);
    }

private:
    const std::string _baseTextureProp = "u_baseTexture";
    const std::string _baseSamplerProp = "u_baseSampler";

    uint32_t _faceIndex{};
    const std::string _faceIndexProp = "u_faceIndex";
};

void IrradianceApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    auto rootEntity = scene->createRootEntity();

    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(0, 0, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
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
    std::array<Entity*, 6> planes{};
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

    planes[0]->transform->setPosition(1, 0, 0);   // PX
    planes[1]->transform->setPosition(-3, 0, 0);  // NX
    planes[2]->transform->setPosition(1, 2, 0);   // PY
    planes[3]->transform->setPosition(1, -2, 0);  // NY
    planes[4]->transform->setPosition(-1, 0, 0);  // PZ
    planes[5]->transform->setPosition(3, 0, 0);   // NZ

    auto ibl_map = ImageManager::GetSingleton().generateIBL("Textures/uffizi_rgba16f_cube.ktx");
    scene->ambientLight()->setSpecularTexture(ibl_map);

    auto changeMip = [&](uint32_t mipLevel) {
        for (uint32_t i = 0; i < 6; i++) {
            auto material = planeMaterials[i];
            material->setBaseTexture(ibl_map->getImageView(wgpu::TextureViewDimension::e2D, mipLevel, i, 1, 1));
            material->setFaceIndex(i);
        }
    };
    changeMip(0);

    scene->play();
}

}  // namespace vox
