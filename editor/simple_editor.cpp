//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "editor/simple_editor.h"

#include "vox.editor/ui/console.h"
#include "vox.editor/ui/hierarchy.h"
#include "vox.editor/ui/inspector.h"
#include "vox.editor/ui/profiler_window.h"
#include "vox.editor/ui/project_settings.h"
#include "vox.editor/ui/tool_bar.h"
#include "vox.editor/view/asset_view.h"
#include "vox.editor/view/game_view.h"
#include "vox.editor/view/scene_view.h"

namespace vox::editor {
void SimpleEditor::setupUI() {
    PanelWindowSettings settings;
    settings.closable = true;
    settings.collapsable = true;
    settings.dockable = true;

    _panelsManager.createPanel<ui::MenuBar>("Menu Bar");
    _panelsManager.createPanel<ui::ProfilerWindow>("Profiler", true, settings, 0.25f);
    _panelsManager.createPanel<ui::Console>("Console", true, settings);
    _panelsManager.createPanel<ui::Hierarchy>("Hierarchy", true, settings);
    _panelsManager.createPanel<ui::Inspector>("Inspector", true, settings);
    _panelsManager.createPanel<ui::SceneView>("Scene View", true, settings, _renderContext.get(),
                                              _sceneManager->currentScene());
    _panelsManager.createPanel<ui::GameView>("Game View", true, settings, _renderContext.get(),
                                             _sceneManager->currentScene());

    _panelsManager.createPanel<ui::AssetView>("Asset View", true, settings, _renderContext.get(),
                                              _sceneManager->currentScene());
    _panelsManager.createPanel<ui::Toolbar>("Toolbar", true, settings, _editorResources.get());
    _panelsManager.createPanel<ui::ProjectSettings>("Project Settings", false, settings, "", "simple");

    _canvas.MakeDockSpace(true);
    _gui->SetCanvas(_canvas);
    _sceneManager->currentScene()->play();
}

void SimpleEditor::updateViews(float deltaTime, wgpu::CommandEncoder& commandEncoder) {
    auto& menuBar = _panelsManager.getPanelAs<ui::MenuBar>("Menu Bar");
    menuBar.handleShortcuts(deltaTime);

    auto& gameView = _panelsManager.getPanelAs<ui::GameView>("Game View");
    auto& sceneView = _panelsManager.getPanelAs<ui::SceneView>("Scene View");
    auto& assetView = _panelsManager.getPanelAs<ui::AssetView>("Asset View");

    {
        assetView.update(deltaTime);
        gameView.update(deltaTime);
        sceneView.update(deltaTime);
    }

    if (assetView.IsOpened()) {
        assetView.render(commandEncoder);
    }

    if (gameView.IsOpened()) {
        gameView.render(commandEncoder);
    }

    if (sceneView.IsOpened()) {
        sceneView.render(commandEncoder);
    }
}

void SimpleEditor::inputEvent(const InputEvent& inputEvent) {
    EditorApplication::inputEvent(inputEvent);

    auto& sceneView = _panelsManager.getPanelAs<ui::SceneView>("Scene View");
    sceneView.inputEvent(inputEvent);
}

}  // namespace vox::editor