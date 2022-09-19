//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/shadow/shadow_manager.h"

namespace vox {
const std::string ShadowManager::_shadowTextureProperty = "u_shadowTexture";
const std::string ShadowManager::_shadowSamplerProperty = "u_shadowSampler";

ShadowManager* ShadowManager::getSingletonPtr() { return ms_singleton; }

ShadowManager& ShadowManager::getSingleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

//-----------------------------------------------------------------------
ShadowManager::ShadowManager(Scene* scene, Camera* camera) : _scene(scene), _camera(camera) {
    _renderPass = std::make_unique<RenderPass>(_scene->device(), _renderPassDescriptor);
    auto shadowSubpass = std::make_unique<CascadedShadowSubpass>(nullptr, _scene, _camera, _depthStencilAttachment);
    _cascadedShadowSubpass = shadowSubpass.get();
    _renderPass->addSubpass(std::move(shadowSubpass));

    _renderPassDescriptor.colorAttachmentCount = 0;
    _renderPassDescriptor.colorAttachments = nullptr;
    _renderPassDescriptor.depthStencilAttachment = &_depthStencilAttachment;

    _depthStencilAttachment.depthLoadOp = wgpu::LoadOp::Clear;
    _depthStencilAttachment.depthClearValue = 1.0;
    _depthStencilAttachment.depthStoreOp = wgpu::StoreOp::Store;

    _depthTexture = std::make_unique<Image>("cascade shadowMap");
    _samplerDescriptor.compare = wgpu::CompareFunction::Less;
    _scene->shaderData.setSampler(ShadowManager::_shadowSamplerProperty, _samplerDescriptor);
}

void ShadowManager::draw(wgpu::CommandEncoder& commandEncoder) {
    _drawSpotShadowMap(commandEncoder);
    _drawDirectShadowMap(commandEncoder);
    _drawPointShadowMap(commandEncoder);
}

void ShadowManager::_drawSpotShadowMap(wgpu::CommandEncoder& commandEncoder) {}

void ShadowManager::_drawDirectShadowMap(wgpu::CommandEncoder& commandEncoder) {
    _cascadedShadowSubpass->_updateShadowSettings();
    _getAvailableRenderTarget();
    _depthStencilAttachment.view = _depthTexture->getImageView()->handle();

    // draw shadow-map
    _renderPass->draw(commandEncoder, "Cascade Shadow Pass");

    _scene->shaderData.setImageView(ShadowManager::_shadowTextureProperty, ShadowManager::_shadowSamplerProperty,
                                    _depthTexture->getImageView());
}

void ShadowManager::_drawPointShadowMap(wgpu::CommandEncoder& commandEncoder) {}

void ShadowManager::_getAvailableRenderTarget() {
    if (_depthTexture->extent().width != uint32_t(_cascadedShadowSubpass->_shadowMapSize.x) ||
        _depthTexture->extent().height != uint32_t(_cascadedShadowSubpass->_shadowMapSize.y) ||
        _depthTexture->format() != _cascadedShadowSubpass->_shadowMapFormat) {
        _depthTexture->setWidth(uint32_t(_cascadedShadowSubpass->_shadowMapSize.x));
        _depthTexture->setHeight(uint32_t(_cascadedShadowSubpass->_shadowMapSize.y));
        _depthTexture->setFormat(_cascadedShadowSubpass->_shadowMapFormat);
        _depthTexture->createTexture(_scene->device(),
                                     wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding);
    }
}

}  // namespace vox
