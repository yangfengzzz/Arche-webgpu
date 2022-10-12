//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "editor/navigation_editor.h"

#include "vox.editor/ui/console.h"
#include "vox.editor/view/asset_view.h"
#include "vox.editor/view/scene_view.h"

namespace vox::editor {
void NavigationEditor::setupUI() {
    PanelWindowSettings settings;
    settings.closable = true;
    settings.collapsable = true;
    settings.dockable = true;

    _panelsManager.createPanel<ui::MenuBar>("Menu Bar");
    _panelsManager.createPanel<ui::Console>("Console", true, settings);
    _panelsManager.createPanel<ui::SceneView>("Scene View", true, settings, _renderContext.get(),
                                              _sceneManager->currentScene());

    _canvas.MakeDockSpace(true);
    _gui->SetCanvas(_canvas);
    _sceneManager->currentScene()->play();
}

void NavigationEditor::updateViews(float deltaTime, wgpu::CommandEncoder& commandEncoder) {
    auto& menuBar = _panelsManager.getPanelAs<ui::MenuBar>("Menu Bar");
    menuBar.handleShortcuts(deltaTime);

    auto& sceneView = _panelsManager.getPanelAs<ui::SceneView>("Scene View");
    sceneView.update(deltaTime);

    if (sceneView.IsOpened()) {
        sceneView.render(commandEncoder);
    }
}

void NavigationEditor::inputEvent(const InputEvent& inputEvent) {
    EditorApplication::inputEvent(inputEvent);

    auto& sceneView = _panelsManager.getPanelAs<ui::SceneView>("Scene View");
    sceneView.inputEvent(inputEvent);
}

}  // namespace vox::editor