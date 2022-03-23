//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "tool_bar.h"
#include "ui/widgets/layout/spacing.h"
#include "editor_actions.h"
#include "editor_resources.h"

namespace vox {
namespace editor {
namespace ui {

Toolbar::Toolbar(const std::string &p_title, bool p_opened,
                 const PanelWindowSettings &p_windowSettings,
                 EditorResources *resource) :
PanelWindow(p_title, p_opened, p_windowSettings),
_resource(resource) {
    std::string iconFolder = ":Textures/Icons/";
    
    _playButton = &createWidget<ButtonImage>(_resource->getTexture("Button_Play"), Vector2F{20, 20});
    _pauseButton = &createWidget<ButtonImage>(_resource->getTexture("Button_Pause"), Vector2F{20, 20});
    _stopButton = &createWidget<ButtonImage>(_resource->getTexture("Button_Stop"), Vector2F{20, 20});
    _nextButton = &createWidget<ButtonImage>(_resource->getTexture("Button_Next"), Vector2F{20, 20});
    
    createWidget<Spacing>(0).lineBreak = false;
    auto &refreshButton = createWidget<ButtonImage>(_resource->getTexture("Button_Refresh"), Vector2F{20, 20});
    
    _playButton->lineBreak = false;
    _pauseButton->lineBreak = false;
    _stopButton->lineBreak = false;
    _nextButton->lineBreak = false;
    refreshButton.lineBreak = false;
    
    _playButton->clickedEvent +=
    std::bind(&EditorActions::startPlaying, EditorActions::getSingletonPtr());
    
    _pauseButton->clickedEvent +=
    std::bind(&EditorActions::pauseGame, EditorActions::getSingletonPtr());
    
    _stopButton->clickedEvent +=
    std::bind(&EditorActions::stopPlaying, EditorActions::getSingletonPtr());
    
    _nextButton->clickedEvent +=
    std::bind(&EditorActions::nextFrame, EditorActions::getSingletonPtr());
    
    refreshButton.clickedEvent +=
    std::bind(&EditorActions::refreshScripts, EditorActions::getSingletonPtr());
    
    EditorActions::getSingleton().editorModeChangedEvent += [this](EditorActions::EditorMode p_newMode) {
        auto enable = [](ButtonImage *p_button, bool p_enable) {
            p_button->disabled = !p_enable;
            p_button->tint = p_enable ? Color{1.0f, 1.0f, 1.0f, 1.0f} : Color{1.0f, 1.0f, 1.0f, 0.15f};
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

void Toolbar::_draw_Impl() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
    
    PanelWindow::_draw_Impl();
    
    ImGui::PopStyleVar();
}


}
}
}
