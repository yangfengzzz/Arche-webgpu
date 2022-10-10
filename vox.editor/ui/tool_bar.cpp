//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/ui/tool_bar.h"

#include "vox.editor/editor_actions.h"
#include "vox.editor/editor_resources.h"
#include "vox.render/ui/widgets/layout/spacing.h"

namespace vox::editor::ui {

Toolbar::Toolbar(const std::string &p_title,
                 bool p_opened,
                 const PanelWindowSettings &p_windowSettings,
                 EditorResources *resource)
    : PanelWindow(p_title, p_opened, p_windowSettings), _resource(resource) {
    std::string iconFolder = ":Textures/Icons/";

    _playButton = &CreateWidget<ButtonImage>(_resource->getTexture("Button_Play"), Vector2F{20, 20});
    _pauseButton = &CreateWidget<ButtonImage>(_resource->getTexture("Button_Pause"), Vector2F{20, 20});
    _stopButton = &CreateWidget<ButtonImage>(_resource->getTexture("Button_Stop"), Vector2F{20, 20});
    _nextButton = &CreateWidget<ButtonImage>(_resource->getTexture("Button_Next"), Vector2F{20, 20});

    CreateWidget<Spacing>(0).line_break_ = false;
    auto &refreshButton = CreateWidget<ButtonImage>(_resource->getTexture("Button_Refresh"), Vector2F{20, 20});

    _playButton->line_break_ = false;
    _pauseButton->line_break_ = false;
    _stopButton->line_break_ = false;
    _nextButton->line_break_ = false;
    refreshButton.line_break_ = false;

    _playButton->clicked_event_ += std::bind(&EditorActions::startPlaying, EditorActions::getSingletonPtr());

    _pauseButton->clicked_event_ += std::bind(&EditorActions::pauseGame, EditorActions::getSingletonPtr());

    _stopButton->clicked_event_ += std::bind(&EditorActions::stopPlaying, EditorActions::getSingletonPtr());

    _nextButton->clicked_event_ += std::bind(&EditorActions::nextFrame, EditorActions::getSingletonPtr());

    refreshButton.clicked_event_ += std::bind(&EditorActions::refreshScripts, EditorActions::getSingletonPtr());

    EditorActions::getSingleton().editorModeChangedEvent += [this](EditorActions::EditorMode p_newMode) {
        auto enable = [](ButtonImage *p_button, bool p_enable) {
            p_button->disabled_ = !p_enable;
            p_button->tint_ = p_enable ? Color{1.0f, 1.0f, 1.0f, 1.0f} : Color{1.0f, 1.0f, 1.0f, 0.15f};
        };

        switch (p_newMode) {
            case EditorActions::EditorMode::EDIT:
                enable(_playButton, true);
                enable(_pauseButton, false);
                enable(_stopButton, false);
                enable(_nextButton, false);
                break;
            case EditorActions::EditorMode::PLAY:
                enable(_playButton, false);
                enable(_pauseButton, true);
                enable(_stopButton, true);
                enable(_nextButton, true);
                break;
            case EditorActions::EditorMode::PAUSE:
                enable(_playButton, true);
                enable(_pauseButton, false);
                enable(_stopButton, true);
                enable(_nextButton, true);
                break;
            case EditorActions::EditorMode::FRAME_BY_FRAME:
                enable(_playButton, true);
                enable(_pauseButton, false);
                enable(_stopButton, true);
                enable(_nextButton, true);
                break;
        }
    };

    EditorActions::getSingleton().setEditorMode(EditorActions::EditorMode::EDIT);
}

void Toolbar::DrawImpl() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));

    PanelWindow::DrawImpl();

    ImGui::PopStyleVar();
}

}  // namespace vox::editor::ui
