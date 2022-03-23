//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "editor_application.h"
#include "engine.h"
#include "camera.h"
#include "glfw_window.h"

#include "ui/menu_bar.h"
#include "ui/hierarchy.h"
#include "ui/inspector.h"
#include "ui/profiler_window.h"
#include "ui/tool_bar.h"
#include "ui/project_settings.h"
#include "ui/console.h"
#include "view/game_view.h"
#include "view/scene_view.h"
#include "view/asset_view.h"

#include "shaderlib/wgsl_grid.h"

namespace vox {
namespace editor {
EditorApplication::EditorApplication(const std::string& projectPath, const std::string& projectName):
GraphicsApplication(),
projectPath(projectPath),
projectName(projectName),
projectFilePath(projectPath + projectName + ".project"),
engineAssetsPath(std::filesystem::canonical("./Data").string() + "/"),
projectAssetsPath(projectPath + "/Assets/"),
projectScriptsPath(projectPath + "/Scripts/"),
editorAssetsPath("/Data/Editor/"),
_panelsManager(_canvas) {
    Shader::create("editor-grid", std::make_unique<WGSLGridVertex>(), std::make_unique<WGSLGridFragment>());
}

bool EditorApplication::prepare(Engine &engine) {
    GraphicsApplication::prepare(engine);
    
    _gui = std::make_unique<::vox::ui::UIManager>(static_cast<GlfwWindow*>(&engine.window())->handle(), _renderContext.get());
    _gui->loadFont("Ruda_Big", "../assets/Fonts/Ruda-Bold.ttf", 16);
    _gui->loadFont("Ruda_Small", "../assets/Fonts/Ruda-Bold.ttf", 12);
    _gui->loadFont("Ruda_Medium", "../assets/Fonts/Ruda-Bold.ttf", 14);
    _gui->useFont("Ruda_Medium");
    _gui->setEditorLayoutAutosaveFrequency(60.0f);
    _gui->enableEditorLayoutSave(true);
    _gui->enableDocking(true);
    
    _sceneManager = std::make_unique<SceneManager>(_device);
    auto scene = _sceneManager->currentScene();
    
    _particleManager = std::make_unique<ParticleManager>(_device);
    _lightManager = std::make_unique<LightManager>(scene);
    {
        auto extent = engine.window().extent();
        auto factor = engine.window().contentScaleFactor();
        scene->updateSize(extent.width, extent.height, factor * extent.width, factor * extent.height);
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
    
    _editorActions = std::make_unique<EditorActions>(_panelsManager);
    _editorResources = std::make_unique<EditorResources>(_device, editorAssetsPath);
    setupUI();
    
    return true;
}

void EditorApplication::setupUI() {
    PanelWindowSettings settings;
    settings.closable = true;
    settings.collapsable = true;
    settings.dockable = true;
    
    _panelsManager.createPanel<ui::MenuBar>("Menu Bar");
    _panelsManager.createPanel<ui::ProfilerWindow>("Profiler", true, settings, 0.25f);
    _panelsManager.createPanel<ui::Console>("Console", true, settings);
    _panelsManager.createPanel<ui::Hierarchy>("Hierarchy", true, settings);
    _panelsManager.createPanel<ui::Inspector>("Inspector", true, settings);
    _panelsManager.createPanel<ui::SceneView>("Scene View", true, settings,
                                              _renderContext.get(), _sceneManager->currentScene());
    _panelsManager.createPanel<ui::GameView>("Game View", true, settings,
                                             _renderContext.get(), _sceneManager->currentScene());
    
    _panelsManager.createPanel<ui::AssetView>("Asset View", true, settings,
                                              _renderContext.get(), _sceneManager->currentScene());
    _panelsManager.createPanel<ui::Toolbar>("Toolbar", true, settings, _editorResources.get());
    _panelsManager.createPanel<ui::ProjectSettings>("Project Settings", false, settings, projectPath, projectName);
    
    _canvas.makeDockspace(true);
    _gui->setCanvas(_canvas);
}

void EditorApplication::renderViews(float deltaTime, wgpu::CommandEncoder& commandEncoder) {
    auto& gameView = _panelsManager.getPanelAs<ui::GameView>("Game View");
    auto& sceneView = _panelsManager.getPanelAs<ui::SceneView>("Scene View");
    auto& assetView = _panelsManager.getPanelAs<ui::AssetView>("Asset View");
    
    {
        // PROFILER_SPY("Editor Views Update");
        assetView.update(deltaTime);
        gameView.update(deltaTime);
        sceneView.update(deltaTime);
    }
    
    if (assetView.isOpened()) {
        // PROFILER_SPY("Game View Rendering");
        assetView.render(commandEncoder);
    }
    
    if (gameView.isOpened()) {
        // PROFILER_SPY("Game View Rendering");
        gameView.render(commandEncoder);
    }
    
    if (sceneView.isOpened()) {
        // PROFILER_SPY("Scene View Rendering");
        sceneView.render(commandEncoder);
    }
}

void EditorApplication::updateEditorPanels(float deltaTime) {
    auto& menuBar = _panelsManager.getPanelAs<ui::MenuBar>("Menu Bar");
    menuBar.handleShortcuts(deltaTime);
}

void EditorApplication::update(float deltaTime) {
    GraphicsApplication::update(deltaTime);
    _sceneManager->currentScene()->update(deltaTime);
    
    wgpu::CommandEncoder commandEncoder = _device.CreateCommandEncoder();
    updateGPUTask(commandEncoder);
    updateEditorPanels(deltaTime);
    renderViews(deltaTime, commandEncoder);
    
    // Render the gui
    _colorAttachments.view = _renderContext->currentDrawableTexture();
    wgpu::RenderPassEncoder encoder = commandEncoder.BeginRenderPass(&_renderPassDescriptor);
    encoder.PushDebugGroup("GUI Rendering");
    _gui->render(encoder);
    encoder.PopDebugGroup();
    encoder.End();
    
    // Finalize rendering here & push the command buffer to the GPU
    wgpu::CommandBuffer commands = commandEncoder.Finish();
    _device.GetQueue().Submit(1, &commands);
    _renderContext->present();
}

void EditorApplication::updateGPUTask(wgpu::CommandEncoder& commandEncoder) {
    _shadowManager->draw(commandEncoder);
    _lightManager->draw(commandEncoder);
    _particleManager->draw(commandEncoder);
}

bool EditorApplication::resize(uint32_t win_width, uint32_t win_height,
                               uint32_t fb_width, uint32_t fb_height) {
    GraphicsApplication::resize(win_width, win_height, fb_width, fb_height);
    
    _sceneManager->currentScene()->updateSize(win_width, win_height, fb_width, fb_height);
    return true;
}

void EditorApplication::inputEvent(const InputEvent &inputEvent) {
    GraphicsApplication::inputEvent(inputEvent);
    _sceneManager->currentScene()->updateInputEvent(inputEvent);
    
    auto& sceneView = _panelsManager.getPanelAs<ui::SceneView>("Scene View");
    sceneView.inputEvent(inputEvent);
}

}
}
