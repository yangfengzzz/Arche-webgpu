//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "game_view.h"
#include "camera.h"
#include "rendering/subpasses/forward_subpass.h"

namespace vox {
namespace editor {
namespace ui {
GameView::GameView(RenderContext* renderContext, Scene* scene,
                   const std::string & p_title, bool p_opened,
                   const PanelWindowSettings & p_windowSettings) :
View(renderContext, p_title, p_opened, p_windowSettings),
_scene(scene) {
    auto editorRoot = _scene->findEntityByName("GameRoot");
    if (!editorRoot) {
        editorRoot = _scene->createRootEntity("GameRoot");
    }
    
    editorRoot->createChild("MainCamera");
    _mainCamera = editorRoot->addComponent<Camera>();
    
    // Create a render pass descriptor for thelighting and composition pass
    // Whatever rendered in the final pass needs to be stored so it can be displayed
    _renderPassDescriptor.colorAttachmentCount = 1;
    _renderPassDescriptor.colorAttachments = &_renderPassColorAttachments;
    _renderPassDescriptor.depthStencilAttachment = &_renderPassDepthStencilAttachment;
    
    _renderPassColorAttachments.storeOp = wgpu::StoreOp::Store;
    _renderPassColorAttachments.loadOp = wgpu::LoadOp::Clear;
    auto& color = scene->background.solidColor;
    _renderPassColorAttachments.clearValue = wgpu::Color{color.r, color.g, color.b, color.a};
    _renderPassDepthStencilAttachment.depthLoadOp = wgpu::LoadOp::Clear;
    _renderPassDepthStencilAttachment.depthClearValue = 1.0;
    _renderPassDepthStencilAttachment.depthStoreOp = wgpu::StoreOp::Discard;
    _renderPassDepthStencilAttachment.stencilLoadOp = wgpu::LoadOp::Clear;
    _renderPassDepthStencilAttachment.stencilStoreOp = wgpu::StoreOp::Discard;
    _renderPass = std::make_unique<RenderPass>(renderContext->device(), _renderPassDescriptor);
    _renderPass->addSubpass(std::make_unique<ForwardSubpass>(_renderContext, _depthStencilTextureFormat,
                                                             scene, _mainCamera));
}

void GameView::update(float p_deltaTime) {
    
}

void GameView::render(wgpu::CommandEncoder& commandEncoder) {
    
}

}
}
}
