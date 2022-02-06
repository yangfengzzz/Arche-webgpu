//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "editor_application.h"
#include "rendering/subpasses/forward_subpass.h"
#include "rendering/subpasses/color_picker_subpass.h"
#include "engine.h"
#include "camera.h"

namespace vox {
EditorApplication::~EditorApplication() {
    _renderPass.reset();
}

bool EditorApplication::prepare(Engine &engine) {
    ForwardApplication::prepare(engine);
    
    auto extent = engine.window().extent();
    auto scale = engine.window().contentScaleFactor();
    
    _colorPickerTextureDesc.format = wgpu::TextureFormat::BGRA8Unorm;
    _colorPickerTextureDesc.size.width = extent.width * scale;
    _colorPickerTextureDesc.size.height = extent.height * scale;
    _colorPickerTextureDesc.mipLevelCount = 1;
    _colorPickerTextureDesc.dimension = wgpu::TextureDimension::e2D;
    _colorPickerTextureDesc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding;
    _colorPickerTexture = _device.CreateTexture(&_colorPickerTextureDesc);
    _colorPickerTexture.SetLabel("ColorPicker Texture");
    
    _colorPickerPassDescriptor.colorAttachmentCount = 1;
    _colorPickerPassDescriptor.colorAttachments = &_colorPickerColorAttachments;
    _colorPickerPassDescriptor.depthStencilAttachment = &_colorPickerDepthStencilAttachment;
    
    _colorPickerColorAttachments.resolveTarget = _colorPickerTexture.CreateView();
    _colorPickerDepthStencilAttachment.depthLoadOp = wgpu::LoadOp::Clear;
    _colorPickerDepthStencilAttachment.clearDepth = 1.0;
    _colorPickerDepthStencilAttachment.view = _renderContext->depthStencilTexture();
    
    _colorPickerRenderPass = std::make_unique<RenderPass>(_device, _colorPickerPassDescriptor);
    auto colorPickerSubpass = std::make_unique<ColorPickerSubpass>(_renderContext.get(), _scene.get(), _mainCamera);
    _colorPickerSubpass = colorPickerSubpass.get();
    _colorPickerRenderPass->addSubpass(std::move(colorPickerSubpass));
    
    return true;
}

void EditorApplication::update(float delta_time) {
    GraphicsApplication::update(delta_time);
    _scene->update(delta_time);
    _scene->updateShaderData();

    wgpu::CommandEncoder commandEncoder = _device.CreateCommandEncoder();
    // _shadowManager->draw(commandBuffer);

    // Render the lighting and composition pass
    _colorAttachments->view = _renderContext->currentDrawableTexture();
    _depthStencilAttachment->view = _renderContext->depthStencilTexture();
    
    _renderPass->draw(commandEncoder, "Lighting & Composition Pass");
    if (_needPick) {
        // _colorPickerRenderPass->draw(commandEncoder, "color Picker Pass");
    }
    
    // Finalize rendering here & push the command buffer to the GPU
    wgpu::CommandBuffer commands = commandEncoder.Finish();
    _device.GetQueue().Submit(1, &commands);
    _renderContext->present();

    if (_needPick) {
        auto picker = _colorPickerSubpass->getObjectByColor(_readColorFromRenderTarget());
        pickFunctor(picker.first, picker.second);
        _needPick = false;
    }
}

bool EditorApplication::resize(uint32_t win_width, uint32_t win_height,
                               uint32_t fb_width, uint32_t fb_height) {
    ForwardApplication::resize(win_width, win_height, fb_width, fb_height);

    _colorPickerTextureDesc.size.width = fb_width;
    _colorPickerTextureDesc.size.height = fb_height;
    _colorPickerTexture = _device.CreateTexture(&_colorPickerTextureDesc);
    
    _colorPickerColorAttachments.resolveTarget = _colorPickerTexture.CreateView();
    _colorPickerDepthStencilAttachment.view = _renderContext->depthStencilTexture();

    return true;
}

void EditorApplication::pick(float offsetX, float offsetY) {
    _needPick = true;
    _pickPos = Vector2F(offsetX, offsetY);
}

std::array<uint8_t, 4> EditorApplication::_readColorFromRenderTarget() {
    int clientWidth = _mainCamera->width();
    int clientHeight = _mainCamera->height();
    int canvasWidth = static_cast<int>(_colorPickerTextureDesc.size.width);
    int canvasHeight = static_cast<int>(_colorPickerTextureDesc.size.width);

    const auto px = (_pickPos.x / clientWidth) * canvasWidth;
    const auto py = (_pickPos.y / clientHeight) * canvasHeight;

    const auto viewport = _mainCamera->viewport();
    const auto viewWidth = (viewport.z - viewport.x) * canvasWidth;
    const auto viewHeight = (viewport.w - viewport.y) * canvasHeight;

    const auto nx = (px - viewport.x) / viewWidth;
    const auto ny = (py - viewport.y) / viewHeight;
    const auto left = std::floor(nx * (canvasWidth - 1));
    const auto bottom = std::floor((1 - ny) * (canvasHeight - 1));
    std::array<uint8_t, 4> pixel;

//    _colorPickerTexture.getBytes(pixel.data(), sizeof(uint8_t) * 4,
//                                 MTL::regionMake2D(left, canvasHeight - bottom, 1, 1), 0);

    return pixel;
}

}
