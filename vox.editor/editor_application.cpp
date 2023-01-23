//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/platform/platform.h"
//
#include <utility>

#include "vox.editor/editor_application.h"
#include "vox.render/camera.h"

namespace vox::editor {
EditorApplication::EditorApplication(std::string editorAssetsPath)
    : GraphicsApplication(), editorAssetsPath(std::move(editorAssetsPath)), _panelsManager(_canvas) {}

EditorApplication::~EditorApplication() {
    // release first
    _sceneManager.reset();

    _componentsManager.reset();
    _lightManager.reset();
    _shadowManager.reset();
    _physicsManager.reset();
    //    _particleManager.reset();

    shader_manager_->CollectGarbage();
    shader_manager_.reset();
    mesh_manager_->CollectGarbage();
    mesh_manager_.reset();
    image_manager_->collectGarbage();
    image_manager_.reset();
    resource_cache_.reset();
}

bool EditorApplication::prepare(Platform& platform) {
    GraphicsApplication::prepare(platform);

    _gui->LoadFont("Ruda_Big", "../assets/Fonts/Ruda-Bold.ttf", 16);
    _gui->LoadFont("Ruda_Small", "../assets/Fonts/Ruda-Bold.ttf", 12);
    _gui->LoadFont("Ruda_Medium", "../assets/Fonts/Ruda-Bold.ttf", 14);
    _gui->UseFont("Ruda_Medium");
    _gui->SetEditorLayoutAutosaveFrequency(60.0f);
    _gui->EnableEditorLayoutSave(true);
    _gui->EnableDocking(true);

    // resource loader
    shader_manager_ = std::make_unique<ShaderManager>();
    mesh_manager_ = std::make_unique<MeshManager>(_device);
    image_manager_ = std::make_unique<ImageManager>(_device);
    resource_cache_ = std::make_unique<ResourceCache>(_device);

    // logic system
    _componentsManager = std::make_unique<ComponentsManager>();
    _physicsManager = std::make_unique<PhysicsManager>();
    _sceneManager = std::make_unique<SceneManager>(_device);
    auto scene = _sceneManager->currentScene();

    //    _particleManager = std::make_unique<ParticleManager>(_device);
    _lightManager = std::make_unique<LightManager>(scene);
    {
        auto extent = platform.GetWindow().GetExtent();
        auto factor = platform.GetWindow().GetContentScaleFactor();
        _componentsManager->callScriptResize(extent.width, extent.height, factor * extent.width,
                                             factor * extent.height);
    }
    _lightManager->setCamera(_mainCamera);
    _shadowManager = std::make_unique<ShadowManager>(scene, _mainCamera);

    // Create a render pass descriptor for thelighting and composition pass
    // Whatever rendered in the final pass needs to be stored so it can be displayed
    _renderPassDescriptor.colorAttachmentCount = 1;
    _renderPassDescriptor.colorAttachments = &_colorAttachments;

    _colorAttachments.storeOp = wgpu::StoreOp::Store;
    _colorAttachments.loadOp = wgpu::LoadOp::Clear;
    auto& color = scene->background.solidColor;
    _colorAttachments.clearValue = wgpu::Color{color.r, color.g, color.b, color.a};

    _editorActions = std::make_unique<EditorActions>(*this);
    _editorResources = std::make_unique<EditorResources>(_device, editorAssetsPath);
    setupUI();

    return true;
}

void EditorApplication::update(float deltaTime) {
    GraphicsApplication::update(deltaTime);
    {
        _componentsManager->callScriptOnStart();

        _physicsManager->update(deltaTime);

        _componentsManager->callScriptOnUpdate(deltaTime);
        _componentsManager->callAnimatorUpdate(deltaTime);
        _componentsManager->callScriptOnLateUpdate(deltaTime);

        _componentsManager->callRendererOnUpdate(deltaTime);
        _sceneManager->currentScene()->updateShaderData();
    }

    wgpu::CommandEncoder commandEncoder = _device.CreateCommandEncoder();
    updateGPUTask(commandEncoder);
    updateViews(deltaTime, commandEncoder);

    // Render the gui
    _colorAttachments.view = _renderContext->currentDrawableTexture();
    wgpu::RenderPassEncoder encoder = commandEncoder.BeginRenderPass(&_renderPassDescriptor);
    encoder.PushDebugGroup("GUI Rendering");
    _gui->Render(encoder);
    encoder.PopDebugGroup();
    encoder.End();

    // Finalize rendering here & push the command buffer to the GPU
    wgpu::CommandBuffer commands = commandEncoder.Finish();
    _device.GetQueue().Submit(1, &commands);
    _renderContext->present();
}

void EditorApplication::updateGPUTask(wgpu::CommandEncoder& commandEncoder) {
    //    _shadowManager->draw(commandEncoder);
    _lightManager->draw(commandEncoder);
    //    _particleManager->draw(commandEncoder);
}

bool EditorApplication::resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) {
    GraphicsApplication::resize(win_width, win_height, fb_width, fb_height);
    _componentsManager->callScriptResize(win_width, win_height, fb_width, fb_height);
    return true;
}

void EditorApplication::inputEvent(const InputEvent& inputEvent) {
    GraphicsApplication::inputEvent(inputEvent);
    _componentsManager->callScriptInputEvent(inputEvent);
}

}  // namespace vox::editor
