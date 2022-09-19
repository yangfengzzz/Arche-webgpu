//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/shadow/shadow_manager.h"

#include "vox.geometry/matrix_utils.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.render/image.h"
#include "vox.render/lighting/light_manager.h"

namespace vox {
ShadowManager* ShadowManager::getSingletonPtr() { return ms_singleton; }

ShadowManager& ShadowManager::getSingleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

//-----------------------------------------------------------------------
ShadowManager::ShadowManager(Scene* scene, Camera* camera) : _scene(scene), _camera(camera) {
    _renderPass = std::make_unique<RenderPass>(_scene->device(), _renderPassDescriptor);
    auto shadowSubpass = std::make_unique<CascadedShadowSubpass>(nullptr, _scene, _camera);
    _cascadedShadowSubpass = shadowSubpass.get();
    _renderPass->addSubpass(std::move(shadowSubpass));

    _renderPassDescriptor.colorAttachmentCount = 0;
    _renderPassDescriptor.colorAttachments = nullptr;
    _renderPassDescriptor.depthStencilAttachment = &_cascadedShadowSubpass->_depthStencilAttachment;
}

void ShadowManager::draw(wgpu::CommandEncoder& commandEncoder) {
    _drawSpotShadowMap(commandEncoder);
    _drawDirectShadowMap(commandEncoder);
    _drawPointShadowMap(commandEncoder);
}

void ShadowManager::_drawSpotShadowMap(wgpu::CommandEncoder& commandEncoder) {}

void ShadowManager::_drawDirectShadowMap(wgpu::CommandEncoder& commandEncoder) {
    _renderPass->draw(commandEncoder, "Direct Shadow Pass");
}

void ShadowManager::_drawPointShadowMap(wgpu::CommandEncoder& commandEncoder) {}

}  // namespace vox
