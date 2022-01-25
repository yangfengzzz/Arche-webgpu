//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "forward_application.h"
#include "rendering/subpasses/forward_subpass.h"
#include "engine.h"
#include "camera.h"

namespace vox {
ForwardApplication::~ForwardApplication() {
    _renderPass.reset();
}

bool ForwardApplication::prepare(Engine &engine) {
    GraphicsApplication::prepare(engine);
    
    _scene = std::make_unique<Scene>(_device);
    
    auto extent = engine.window().extent();
    loadScene(extent.width, extent.height);
    
    //    _shadowManager = std::make_unique<ShadowManager>(_library, _scene.get(), _mainCamera);
    
    // Create a render pass descriptor for thelighting and composition pass
    // Whatever rendered in the final pass needs to be stored so it can be displayed
    _colorAttachments = std::make_unique<wgpu::RenderPassColorAttachment>();
    _depthStencilAttachment = std::make_unique<wgpu::RenderPassDepthStencilAttachment>();
    _renderPassDescriptor.colorAttachmentCount = 1;
    _renderPassDescriptor.colorAttachments = _colorAttachments.get();
    _renderPassDescriptor.depthStencilAttachment = _depthStencilAttachment.get();
    
    _colorAttachments->storeOp = wgpu::StoreOp::Store;
    _colorAttachments->loadOp = wgpu::LoadOp::Clear;
    auto& color = _scene->background.solidColor;
    _colorAttachments->clearColor = wgpu::Color{color.r, color.g, color.b, color.a};
    _depthStencilAttachment->depthLoadOp = wgpu::LoadOp::Clear;
    _depthStencilAttachment->stencilLoadOp = wgpu::LoadOp::Clear;
    _renderPass = std::make_unique<RenderPass>(_renderPassDescriptor);
    _renderPass->addSubpass(std::make_unique<ForwardSubpass>(_renderContext.get(), _scene.get(), _mainCamera));
    //    if (_gui) {
    //        _renderPass->setGUI(_gui.get());
    //    }
    
    return true;
}

void ForwardApplication::update(float delta_time) {
    GraphicsApplication::update(delta_time);
    _scene->update(delta_time);
    _scene->updateShaderData();
    
    wgpu::CommandEncoder commandEncoder = _device.CreateCommandEncoder();
    //    _shadowManager->draw(commandBuffer);
    
    // Render the lighting and composition pass
    _colorAttachments->view = _renderContext->currentDrawableTexture();
    _depthStencilAttachment->view = _renderContext->depthStencilTexture();
    
    _renderPass->draw(commandEncoder, "Lighting & Composition Pass");
    // Finalize rendering here & push the command buffer to the GPU
    wgpu::CommandBuffer commands = commandEncoder.Finish();
    _queue.Submit(1, &commands);
    _renderContext->present();
}

bool ForwardApplication::resize(uint32_t win_width, uint32_t win_height,
                                uint32_t fb_width, uint32_t fb_height) {
    GraphicsApplication::resize(win_width, win_height, fb_width, fb_height);
    _scene->updateSize(win_width, win_height, fb_width, fb_height);
    _mainCamera->resize(win_width, win_height);
    return true;
}

void ForwardApplication::inputEvent(const InputEvent &inputEvent) {
    GraphicsApplication::inputEvent(inputEvent);
    _scene->updateInputEvent(inputEvent);
}

}
