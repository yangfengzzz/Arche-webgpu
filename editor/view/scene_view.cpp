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
    
    auto grid = rootEntity->addComponent<MeshRenderer>();
    grid->setMesh(createPlane(_renderContext->device()));
    grid->setMaterial(std::make_shared<GridMaterial>(_renderContext->device()));

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
    if (winWidth > 0 && _colorPickerTextureDesc.size.width != winWidth * 2) {
        _mainCamera->setAspectRatio(float(winWidth) / float(winHeight));
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
    
    // Let the first frame happen and then make the scene view the first seen view
    if (_elapsedFrames) {
        focus();
    }
    
    if (_texture && isFocused()) {
        _elapsedFrames = false;

        _renderPassColorAttachments.view = _texture.CreateView();
        _renderPassDepthStencilAttachment.view = _depthStencilTexture;
        _renderPass->draw(commandEncoder);
        
        if (_needPick) {
            _colorPickerColorAttachments.view = _colorPickerTexture.CreateView();
            _colorPickerDepthStencilAttachment.view = _depthStencilTexture;
            _colorPickerRenderPass->draw(commandEncoder, "color Picker Pass");
            _copyRenderTargetToBuffer(commandEncoder);
        }
        
        _renderContext->device().GetQueue().OnSubmittedWorkDone(0, [](WGPUQueueWorkDoneStatus status, void * userdata) {
            if (status == WGPUQueueWorkDoneStatus_Success) {
                SceneView* app = static_cast<SceneView*>(userdata);
                if (app->_needPick) {
                    app->_readColorFromRenderTarget();
                    app->_needPick = false;
                }
            }
        }, this);
    }
}

void SceneView::_draw_Impl() {
    View::_draw_Impl();
    int windowFlags = ImGuiWindowFlags_None;
    
    if (!resizable) windowFlags |= ImGuiWindowFlags_NoResize;
    if (!movable) windowFlags |= ImGuiWindowFlags_NoMove;
    if (!dockable) windowFlags |= ImGuiWindowFlags_NoDocking;
    if (hideBackground) windowFlags |= ImGuiWindowFlags_NoBackground;
    if (forceHorizontalScrollbar) windowFlags |= ImGuiWindowFlags_AlwaysHorizontalScrollbar;
    if (forceVerticalScrollbar) windowFlags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
    if (allowHorizontalScrollbar) windowFlags |= ImGuiWindowFlags_HorizontalScrollbar;
    if (!bringToFrontOnFocus) windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (!collapsable) windowFlags |= ImGuiWindowFlags_NoCollapse;
    if (!allowInputs) windowFlags |= ImGuiWindowFlags_NoInputs;
    if (!scrollable) windowFlags |= ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;
    if (!titleBar) windowFlags |= ImGuiWindowFlags_NoTitleBar;
    
    if (ImGui::Begin((name + _panelID).c_str(), nullptr, windowFlags)) {
        if (_pickResult.first != nullptr) {
            if (ImGuizmo::IsOver()) {
                _cameraControl->setEnabled(false);
            }
            
            auto renderer = _pickResult.first;
            auto cameraProjection = _mainCamera->projectionMatrix();
            auto cameraView = _mainCamera->viewMatrix();
            auto modelMat = renderer->entity()->transform->localMatrix();
            editTransform(cameraView.data(), cameraProjection.data(), modelMat.data());
            renderer->entity()->transform->setLocalMatrix(modelMat);
            cameraView.invert();
            _mainCamera->entity()->transform->setWorldMatrix(cameraView);
        }
    }
    ImGui::End();
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
            auto result = app->_colorPickerSubpass->getObjectByColor(app->_pixel);
            if (result.first) {
                app->_pickResult = result;
            }
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
            
            auto pickerPosX = mouse_button.pos_x() - position().x;
            auto pickerPosY = mouse_button.pos_y() - position().y;

            if (pickerPosX <= width && pickerPosX > 0 && pickerPosY <= height && pickerPosY > 0) {
                pick(pickerPosX, pickerPosY);
            }
        }
    }
}

void SceneView::editTransform(float *cameraView, float *cameraProjection, float *matrix) {
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
    static bool useSnap = false;
    static float snap[3] = {1.f, 1.f, 1.f};
    static float bounds[] = {-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f};
    static float boundsSnap[] = {0.1f, 0.1f, 0.1f};
    static bool boundSizing = false;
    static bool boundSizingSnap = false;
    
    auto [winWidth, winHeight] = safeSize();
    auto viewportPos = position();
    float viewManipulateRight = winWidth + viewportPos.x;
    float viewManipulateTop = viewportPos.y + 25;
    ImGuizmo::SetRect(viewportPos.x, viewportPos.y, winWidth, winHeight);
    ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());

    ImGuizmo::Manipulate(cameraView, cameraProjection, _currentGizmoOperation, mCurrentGizmoMode,
                         matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);
    
    ImGuizmo::ViewManipulate(cameraView, _camDistance, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);
}

}
}
}
