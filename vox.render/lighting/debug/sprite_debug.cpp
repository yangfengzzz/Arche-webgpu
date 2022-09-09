//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/lighting/debug/sprite_debug.h"

#include "vox.render/entity.h"
#include "vox.render/lighting/light_manager.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/shader/shader_manager.h"

namespace vox {
// MARK: - SpriteDebugMaterial
SpriteDebugMaterial::SpriteDebugMaterial(wgpu::Device& device, bool isSpotLight) : BaseMaterial(device) {
    setIsTransparent(true);
    setBlendMode(BlendMode::Additive);

    vertex_source_ = ShaderManager::GetSingleton().LoadShader("light/light_sprite.vert");
    fragment_source_ = ShaderManager::GetSingleton().LoadShader("light/light_sprite.frag");
}

std::string SpriteDebug::name() { return "SpriteDebug"; }

SpriteDebug::SpriteDebug(Entity* entity) : Script(entity) {
    _spotLightMesh = std::make_shared<BufferMesh>();
    _spotLightMesh->addSubMesh(0, 4, wgpu::PrimitiveTopology::TriangleStrip);
    _spotEntity = entity->createChild();
    auto spotRenderer = _spotEntity->addComponent<MeshRenderer>();
    spotRenderer->setMaterial(std::make_shared<SpriteDebugMaterial>(entity->scene()->device(), true));
    spotRenderer->setMesh(_spotLightMesh);

    _pointLightMesh = std::make_shared<BufferMesh>();
    _pointLightMesh->addSubMesh(0, 4, wgpu::PrimitiveTopology::TriangleStrip);
    _pointEntity = entity->createChild();
    auto pointRenderer = _pointEntity->addComponent<MeshRenderer>();
    pointRenderer->setMaterial(std::make_shared<SpriteDebugMaterial>(entity->scene()->device(), false));
    pointRenderer->setMesh(_pointLightMesh);
}

void SpriteDebug::onUpdate(float deltaTime) {
    auto spotLightCount = LightManager::getSingleton().spotLights().size();
    if (spotLightCount > 0) {
        _spotLightMesh->setInstanceCount(static_cast<uint32_t>(spotLightCount));
        _spotEntity->setIsActive(true);
    } else {
        _spotEntity->setIsActive(false);
    }

    auto pointLightCount = LightManager::getSingleton().pointLights().size();
    if (pointLightCount > 0) {
        _pointLightMesh->setInstanceCount(static_cast<uint32_t>(pointLightCount));
        _pointEntity->setIsActive(true);
    } else {
        _pointEntity->setIsActive(false);
    }
}

}  // namespace vox
