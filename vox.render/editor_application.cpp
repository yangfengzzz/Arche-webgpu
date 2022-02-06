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
    _colorPickerTextureDesc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::CopySrc;
    _colorPickerTexture = _device.CreateTexture(&_colorPickerTextureDesc);
    _colorPickerTexture.SetLabel("ColorPicker Texture");
    
    _colorPickerPassDescriptor.colorAttachmentCount = 1;
    _colorPickerPassDescriptor.colorAttachments = &_colorPickerColorAttachments;
    _colorPickerPassDescriptor.depthStencilAttachment = &_colorPickerDepthStencilAttachment;
    
    _colorPickerColorAttachments.storeOp = wgpu::StoreOp::Store;
    _colorPickerColorAttachments.loadOp = wgpu::LoadOp::Clear;
    _colorPickerDepthStencilAttachment.depthLoadOp = wgpu::LoadOp::Clear;
    _colorPickerDepthStencilAttachment.clearDepth = 1.0;
    _colorPickerDepthStencilAttachment.depthStoreOp = wgpu::StoreOp::Discard;
    _colorPickerDepthStencilAttachment.stencilStoreOp = wgpu::StoreOp::Discard;
    
    _colorPickerRenderPass = std::make_unique<RenderPass>(_device, _colorPickerPassDescriptor);
    auto colorPickerSubpass = std::make_unique<ColorPickerSubpass>(_renderContext.get(), _scene.get(), _mainCamera);
    _colorPickerSubpass = colorPickerSubpass.get();
    _colorPickerRenderPass->addSubpass(std::move(colorPickerSubpass));
    
    wgpu::BufferDescriptor bufferDesc;
    bufferDesc.usage = wgpu::BufferUsage::MapRead | wgpu::BufferUsage::CopyDst;
    bufferDesc.size = 4;
    _stageBuffer = _device.CreateBuffer(&bufferDesc);
    
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
        _colorPickerColorAttachments.view = _colorPickerTexture.CreateView();
        _colorPickerDepthStencilAttachment.view = _renderContext->depthStencilTexture();
        _colorPickerRenderPass->draw(commandEncoder, "color Picker Pass");
        _copyRenderTargetToBuffer(commandEncoder);
    }
    
    // Finalize rendering here & push the command buffer to the GPU
    wgpu::CommandBuffer commands = commandEncoder.Finish();
    _device.GetQueue().Submit(1, &commands);
    _renderContext->present();

    if (_needPick) {
        _readColorFromRenderTarget();
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

void EditorApplication::_copyRenderTargetToBuffer(wgpu::CommandEncoder& commandEncoder) {
    uint32_t clientWidth = _mainCamera->width();
    uint32_t clientHeight = _mainCamera->height();
    uint32_t canvasWidth = static_cast<uint32_t>(_colorPickerTextureDesc.size.width);
    uint32_t canvasHeight = static_cast<uint32_t>(_colorPickerTextureDesc.size.height);

    const float px = (_pickPos.x / clientWidth) * canvasWidth;
    const float py = (_pickPos.y / clientHeight) * canvasHeight;

    const auto viewport = _mainCamera->viewport();
    const auto viewWidth = (viewport.z - viewport.x) * canvasWidth;
    const auto viewHeight = (viewport.w - viewport.y) * canvasHeight;

    const float nx = (px - viewport.x) / viewWidth;
    const float ny = (py - viewport.y) / viewHeight;
    const uint32_t left = std::floor(nx * (canvasWidth - 1));
    const uint32_t bottom = std::floor((1 - ny) * (canvasHeight - 1));
    
    wgpu::ImageCopyTexture imageCopyTexture;
    imageCopyTexture.texture = _colorPickerTexture;
    imageCopyTexture.mipLevel = 0;
    imageCopyTexture.origin = wgpu::Origin3D{left, canvasHeight - bottom, 0};
    imageCopyTexture.aspect = wgpu::TextureAspect::All;
        
    wgpu::ImageCopyBuffer imageCopyBuffer;
    imageCopyBuffer.buffer = _stageBuffer;
    imageCopyBuffer.layout.offset = 0;
    imageCopyBuffer.layout.bytesPerRow = sizeof(uint8_t) * 4 * _colorPickerTextureDesc.size.width;
    
    wgpu::Extent3D extent{1, 1, 0};
    commandEncoder.CopyTextureToBuffer(&imageCopyTexture, &imageCopyBuffer, &extent);
}

void EditorApplication::_readColorFromRenderTarget() {
    _stageBuffer.MapAsync(wgpu::MapMode::Read, 0, 4, [](WGPUBufferMapAsyncStatus status, void * userdata) {
        if (status == WGPUBufferMapAsyncStatus_Success) {
            EditorApplication* app = static_cast<EditorApplication*>(userdata);
            memcpy(app->_pixel.data(), app->_stageBuffer.GetConstMappedRange(0, 4), 4);
            auto picker = app->_colorPickerSubpass->getObjectByColor(app->_pixel);
            app->pickFunctor(picker.first, picker.second);
            
            app->_stageBuffer.Unmap();
        }
    }, this);
}

}
