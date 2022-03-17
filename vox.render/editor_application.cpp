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
    GraphicsApplication::prepare(engine);
    
    auto extent = engine.window().extent();
    auto scale = engine.window().contentScaleFactor();
    
    _scene = std::make_unique<Scene>(_device);
    _particleManager = std::make_unique<ParticleManager>(_device);
    _lightManager = std::make_unique<LightManager>(_scene.get());
    {
        loadScene();
        auto extent = engine.window().extent();
        auto factor = engine.window().contentScaleFactor();
        _scene->updateSize(extent.width, extent.height, factor * extent.width, factor * extent.height);
        _mainCamera->resize(extent.width, extent.height, factor * extent.width, factor * extent.height);
    }
    _lightManager->setCamera(_mainCamera);
    _shadowManager = std::make_unique<ShadowManager>(_scene.get(), _mainCamera);
    
    // scene render target
    {
        // Create a render pass descriptor for thelighting and composition pass
        // Whatever rendered in the final pass needs to be stored so it can be displayed
        _renderPassDescriptor.colorAttachmentCount = 1;
        _renderPassDescriptor.colorAttachments = &_colorAttachments;
        _renderPassDescriptor.depthStencilAttachment = &_depthStencilAttachment;
        
        _sceneTextureDesc.format = _renderContext->drawableTextureFormat();
        _sceneTextureDesc.size.width = extent.width * scale;
        _sceneTextureDesc.size.height = extent.height * scale;
        _sceneTextureDesc.mipLevelCount = 1;
        _sceneTextureDesc.dimension = wgpu::TextureDimension::e2D;
        _sceneTextureDesc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding;
        _sceneTexture = _device.CreateTexture(&_sceneTextureDesc);
        _sceneTexture.SetLabel("Scene Texture");
        
        _colorAttachments.storeOp = wgpu::StoreOp::Store;
        _colorAttachments.loadOp = wgpu::LoadOp::Clear;
        auto& color = _scene->background.solidColor;
        _colorAttachments.clearValue = wgpu::Color{color.r, color.g, color.b, color.a};
        _depthStencilAttachment.depthLoadOp = wgpu::LoadOp::Clear;
        _depthStencilAttachment.depthClearValue = 1.0;
        _depthStencilAttachment.depthStoreOp = wgpu::StoreOp::Discard;
        _depthStencilAttachment.stencilLoadOp = wgpu::LoadOp::Clear;
        _depthStencilAttachment.stencilStoreOp = wgpu::StoreOp::Discard;
        
        _renderPass = std::make_unique<RenderPass>(_device, _renderPassDescriptor);
        _renderPass->addSubpass(std::make_unique<ForwardSubpass>(_renderContext.get(), _scene.get(), _mainCamera));
    }
    
    // color picker render target
    {
        _colorPickerPassDescriptor.colorAttachmentCount = 1;
        _colorPickerPassDescriptor.colorAttachments = &_colorPickerColorAttachments;
        _colorPickerPassDescriptor.depthStencilAttachment = &_colorPickerDepthStencilAttachment;
        
        _colorPickerTextureDesc.format = wgpu::TextureFormat::BGRA8Unorm;
        _colorPickerTextureDesc.size.width = extent.width * scale;
        _colorPickerTextureDesc.size.height = extent.height * scale;
        _colorPickerTextureDesc.mipLevelCount = 1;
        _colorPickerTextureDesc.dimension = wgpu::TextureDimension::e2D;
        _colorPickerTextureDesc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::CopySrc;
        _colorPickerTexture = _device.CreateTexture(&_colorPickerTextureDesc);
        _colorPickerTexture.SetLabel("ColorPicker Texture");
        
        _colorPickerColorAttachments.storeOp = wgpu::StoreOp::Store;
        _colorPickerColorAttachments.loadOp = wgpu::LoadOp::Clear;
        _colorPickerColorAttachments.clearValue = wgpu::Color{1, 1, 1, 1};
        _colorPickerDepthStencilAttachment.depthLoadOp = wgpu::LoadOp::Clear;
        _colorPickerDepthStencilAttachment.depthClearValue = 1.0;
        _colorPickerDepthStencilAttachment.depthStoreOp = wgpu::StoreOp::Discard;
        _colorPickerDepthStencilAttachment.stencilLoadOp = wgpu::LoadOp::Clear;
        _colorPickerDepthStencilAttachment.stencilStoreOp = wgpu::StoreOp::Discard;
        
        _colorPickerRenderPass = std::make_unique<RenderPass>(_device, _colorPickerPassDescriptor);
        auto colorPickerSubpass = std::make_unique<ColorPickerSubpass>(_renderContext.get(), _scene.get(), _mainCamera);
        _colorPickerSubpass = colorPickerSubpass.get();
        _colorPickerRenderPass->addSubpass(std::move(colorPickerSubpass));
    }
    
    // gui
    {
        // Create a render pass descriptor for thelighting and composition pass
        // Whatever rendered in the final pass needs to be stored so it can be displayed
        _guiPassDescriptor.colorAttachmentCount = 1;
        _guiPassDescriptor.colorAttachments = &_guiColorAttachments;
        
        _guiColorAttachments.storeOp = wgpu::StoreOp::Store;
        _guiColorAttachments.loadOp = wgpu::LoadOp::Clear;
    }
    
    wgpu::BufferDescriptor bufferDesc;
    bufferDesc.usage = wgpu::BufferUsage::MapRead | wgpu::BufferUsage::CopyDst;
    bufferDesc.size = 4;
    _stageBuffer = _device.CreateBuffer(&bufferDesc);
    
    _imageCopyTexture.texture = _colorPickerTexture;
    _imageCopyTexture.mipLevel = 0;
    _imageCopyTexture.aspect = wgpu::TextureAspect::All;
    
    _imageCopyBuffer.buffer = _stageBuffer;
    _imageCopyBuffer.layout.offset = 0;
    
    _extent.width = 1;
    _extent.height = 1;
    
    return true;
}

void EditorApplication::update(float delta_time) {
    GraphicsApplication::update(delta_time);
    
    // Render the lighting and composition pass
    _colorAttachments.view = _sceneTexture.CreateView();
    _depthStencilAttachment.view = _renderContext->depthStencilTexture();
    _scene->update(delta_time);

    wgpu::CommandEncoder commandEncoder = _device.CreateCommandEncoder();
    updateGPUTask(commandEncoder);
    _renderPass->draw(commandEncoder, "Lighting & Composition Pass");
    
    if (_needPick) {
        _colorPickerColorAttachments.view = _colorPickerTexture.CreateView();
        _colorPickerDepthStencilAttachment.view = _renderContext->depthStencilTexture();
        _colorPickerRenderPass->draw(commandEncoder, "color Picker Pass");
        _copyRenderTargetToBuffer(commandEncoder);
    }
    
    
    if (_gui) {
        editorUpdate(_colorAttachments.view);
        ImDrawData *drawData = ImGui::GetDrawData();
        if (drawData) {
            _guiColorAttachments.view = _renderContext->currentDrawableTexture();
            wgpu::RenderPassEncoder encoder = commandEncoder.BeginRenderPass(&_guiPassDescriptor);
            encoder.PushDebugGroup("GUI Rendering");
            _gui->draw(drawData, encoder);
            encoder.PopDebugGroup();
            encoder.End();
        }
    }
    
    // Finalize rendering here & push the command buffer to the GPU
    wgpu::CommandBuffer commands = commandEncoder.Finish();
    _device.GetQueue().OnSubmittedWorkDone(0, [](WGPUQueueWorkDoneStatus status, void * userdata) {
        if (status == WGPUQueueWorkDoneStatus_Success) {
            EditorApplication* app = static_cast<EditorApplication*>(userdata);
            if (app->_needPick) {
                app->_readColorFromRenderTarget();
                app->_needPick = false;
            }
        }
    }, this);
    _device.GetQueue().Submit(1, &commands);
    _renderContext->present();
}

bool EditorApplication::resize(uint32_t win_width, uint32_t win_height,
                               uint32_t fb_width, uint32_t fb_height) {
    ForwardApplication::resize(win_width, win_height, fb_width, fb_height);
    
    _colorPickerTextureDesc.size.width = fb_width;
    _colorPickerTextureDesc.size.height = fb_height;
    _colorPickerTexture = _device.CreateTexture(&_colorPickerTextureDesc);
    _colorPickerColorAttachments.view = _colorPickerTexture.CreateView();
    _colorPickerDepthStencilAttachment.view = _renderContext->depthStencilTexture();
    
    _sceneTextureDesc.size.width = fb_width;
    _sceneTextureDesc.size.height = fb_height;
    _sceneTexture = _device.CreateTexture(&_sceneTextureDesc);
    _colorAttachments.view = _sceneTexture.CreateView();
    
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
    
    _imageCopyTexture.origin = wgpu::Origin3D{left, canvasHeight - bottom, 0};
    commandEncoder.CopyTextureToBuffer(&_imageCopyTexture, &_imageCopyBuffer, &_extent);
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
