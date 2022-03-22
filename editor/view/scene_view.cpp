//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "scene_view.h"
#include "camera.h"
#include "rendering/subpasses/forward_subpass.h"
#include "rendering/subpasses/color_picker_subpass.h"

#include "lighting/point_light.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/unlit_material.h"
#include "material/blinn_phong_material.h"

namespace vox {
namespace editor {
namespace ui {
SceneView::SceneView(const std::string & p_title, bool p_opened,
                   const PanelWindowSettings & p_windowSettings,
                   RenderContext* renderContext, Scene* scene) :
View(p_title, p_opened, p_windowSettings, renderContext),
_scene(scene) {
    scene->background.solidColor = Color(0.2, 0.4, 0.6, 1.0);
    auto editorRoot = _scene->findEntityByName("SceneRoot");
    if (!editorRoot) {
        editorRoot = _scene->createRootEntity("SceneRoot");
    }
    loadScene(editorRoot);
    
    // scene render target
    {
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
    
    // color picker render target
    {
        _colorPickerPassDescriptor.colorAttachmentCount = 1;
        _colorPickerPassDescriptor.colorAttachments = &_colorPickerColorAttachments;
        _colorPickerPassDescriptor.depthStencilAttachment = &_colorPickerDepthStencilAttachment;
        
        _colorPickerColorAttachments.storeOp = wgpu::StoreOp::Store;
        _colorPickerColorAttachments.loadOp = wgpu::LoadOp::Clear;
        _colorPickerColorAttachments.clearValue = wgpu::Color{1, 1, 1, 1};
        _colorPickerDepthStencilAttachment.depthLoadOp = wgpu::LoadOp::Clear;
        _colorPickerDepthStencilAttachment.depthClearValue = 1.0;
        _colorPickerDepthStencilAttachment.depthStoreOp = wgpu::StoreOp::Discard;
        _colorPickerDepthStencilAttachment.stencilLoadOp = wgpu::LoadOp::Clear;
        _colorPickerDepthStencilAttachment.stencilStoreOp = wgpu::StoreOp::Discard;
        
        _colorPickerRenderPass = std::make_unique<RenderPass>(_renderContext->device(), _colorPickerPassDescriptor);
        auto colorPickerSubpass = std::make_unique<ColorPickerSubpass>(_renderContext, _depthStencilTextureFormat,
                                                                       scene, _mainCamera);
        _colorPickerSubpass = colorPickerSubpass.get();
        _colorPickerRenderPass->addSubpass(std::move(colorPickerSubpass));
    }
    
    wgpu::BufferDescriptor bufferDesc;
    bufferDesc.usage = wgpu::BufferUsage::MapRead | wgpu::BufferUsage::CopyDst;
    bufferDesc.size = 4;
    _stageBuffer = _renderContext->device().CreateBuffer(&bufferDesc);
    
    _imageCopyTexture.mipLevel = 0;
    _imageCopyTexture.aspect = wgpu::TextureAspect::All;
    
    _imageCopyBuffer.buffer = _stageBuffer;
    _imageCopyBuffer.layout.offset = 0;
    
    _extent.width = 1;
    _extent.height = 1;
}

void SceneView::loadScene(EntityPtr& rootEntity) {
    auto cameraEntity = rootEntity->createChild("MainCamera");
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    _cameraControl = cameraEntity->addComponent<control::OrbitControl>();

    // init point light
    auto light = rootEntity->createChild("light");
    light->transform->setPosition(0, 3, 0);
    auto pointLight = light->addComponent<PointLight>();
    pointLight->intensity = 0.3;
    
    // create box test entity
    float cubeSize = 2.0;
    auto boxEntity = rootEntity->createChild("BoxEntity");
    auto boxMtl = std::make_shared<BlinnPhongMaterial>(_renderContext->device());
    auto boxRenderer = boxEntity->addComponent<MeshRenderer>();
    boxMtl->setBaseColor(Color(0.8, 0.3, 0.3, 1.0));
    boxRenderer->setMesh(PrimitiveMesh::createCuboid(_renderContext->device(), cubeSize, cubeSize, cubeSize));
    boxRenderer->setMaterial(boxMtl);
}

void SceneView::update(float deltaTime) {
    View::update(deltaTime);
    
    auto [winWidth, winHeight] = safeSize();
    if (winWidth > 0) {
        _mainCamera->resize(winWidth, winHeight, winWidth * 2, winHeight * 2);
        _colorPickerTextureDesc.format = wgpu::TextureFormat::BGRA8Unorm;
        _colorPickerTextureDesc.size.width = winWidth * 2;
        _colorPickerTextureDesc.size.height = winHeight * 2;
        _colorPickerTextureDesc.mipLevelCount = 1;
        _colorPickerTextureDesc.dimension = wgpu::TextureDimension::e2D;
        _colorPickerTextureDesc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::CopySrc;
        _colorPickerTexture = _renderContext->device().CreateTexture(&_colorPickerTextureDesc);
        _colorPickerTexture.SetLabel("ColorPicker Texture");
    }
}

void SceneView::render(wgpu::CommandEncoder& commandEncoder) {
    if (isFocused()) {
        _cameraControl->setEnabled(true);
    } else {
        _cameraControl->setEnabled(false);
    }
    
    if (_texture) {
        _renderPassColorAttachments.view = _texture.CreateView();
        _renderPassDepthStencilAttachment.view = _depthStencilTexture;
        _renderPass->draw(commandEncoder);
        
        if (_needPick) {
            _colorPickerColorAttachments.view = _colorPickerTexture.CreateView();
            _colorPickerDepthStencilAttachment.view = _depthStencilTexture;
            _colorPickerRenderPass->draw(commandEncoder, "color Picker Pass");
            _copyRenderTargetToBuffer(commandEncoder);
        }
    }
}

void SceneView::pick(float offsetX, float offsetY) {
    _needPick = true;
    _pickPos = Vector2F(offsetX, offsetY);
}

void SceneView::_copyRenderTargetToBuffer(wgpu::CommandEncoder& commandEncoder) {
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
    
    _imageCopyTexture.texture = _colorPickerTexture;
    _imageCopyTexture.origin = wgpu::Origin3D{left, canvasHeight - bottom, 0};
    commandEncoder.CopyTextureToBuffer(&_imageCopyTexture, &_imageCopyBuffer, &_extent);
}

void SceneView::_readColorFromRenderTarget() {
    _stageBuffer.MapAsync(wgpu::MapMode::Read, 0, 4, [](WGPUBufferMapAsyncStatus status, void * userdata) {
        if (status == WGPUBufferMapAsyncStatus_Success) {
            SceneView* app = static_cast<SceneView*>(userdata);
            memcpy(app->_pixel.data(), app->_stageBuffer.GetConstMappedRange(0, 4), 4);
            app->_pickResult = app->_colorPickerSubpass->getObjectByColor(app->_pixel);
            app->_stageBuffer.Unmap();
        }
    }, this);
}

void SceneView::inputEvent(const InputEvent &inputEvent) {
    if (inputEvent.source() == EventSource::Mouse) {
        const auto &mouse_button = static_cast<const MouseButtonInputEvent &>(inputEvent);
        if (mouse_button.action() == MouseAction::Down) {
            auto width = _mainCamera->width();
            auto height = _mainCamera->height();
            
            auto pickerUnitPosX = mouse_button.pos_x() - position().x;
            auto pickerUnitPosY = mouse_button.pos_y() - position().y;

            if (pickerUnitPosX <= 1 && pickerUnitPosX > 0 && pickerUnitPosY <= 1 && pickerUnitPosY > 0) {
                pick(pickerUnitPosX * width, pickerUnitPosY * height);
            }
        }
    }
}


}
}
}
