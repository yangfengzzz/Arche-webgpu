//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "menu_bar.h"

#include "ui/widgets/visual/separator.h"
#include "ui/widgets/sliders/slider_int.h"
#include "ui/widgets/sliders/slider_float.h"
#include "ui/widgets/drags/drag_float.h"
#include "ui/widgets/selection/color_edit.h"
#include "ui/widgets/texts/text.h"

#include "editor_utils.h"

namespace vox {
namespace editor {
namespace ui {
MenuBar::MenuBar() {
    createFileMenu();
    createBuildMenu();
    createWindowMenu();
    createActorsMenu();
    createResourcesMenu();
    createSettingsMenu();
    createLayoutMenu();
    createHelpMenu();
}

void MenuBar::handleShortcuts(float p_deltaTime) {
    
}

void MenuBar::registerPanel(const std::string &p_name, PanelWindow &p_panel) {
    
}

void MenuBar::createFileMenu() {
    
}

void MenuBar::createBuildMenu() {
    
}

void MenuBar::createWindowMenu() {
    
}

void MenuBar::createActorsMenu() {
    
}

void MenuBar::createResourcesMenu() {
    
}

void MenuBar::createSettingsMenu() {
    
}

void MenuBar::createLayoutMenu() {
    
}

void MenuBar::createHelpMenu() {
    auto& helpMenu = createWidget<MenuList>("Help");
    helpMenu.createWidget<MenuItem>("GitHub").clickedEvent += [] {
        openURL("https://github.com/adriengivry/Overload");
    };
    helpMenu.createWidget<MenuItem>("Tutorials").clickedEvent += [] {
        openURL("https://github.com/adriengivry/Overload/wiki/Tutorials");
    };
    helpMenu.createWidget<MenuItem>("Scripting API").clickedEvent += [] {
        openURL("https://github.com/adriengivry/Overload/wiki/Scripting-API");
    };
    helpMenu.createWidget<Separator>();
    helpMenu.createWidget<MenuItem>("Bug Report").clickedEvent += [] {
        openURL("https://github.com/adriengivry/Overload/issues/new?assignees=&labels=Bug&template=bug_report.md&title=");
    };
    helpMenu.createWidget<MenuItem>("Feature Request").clickedEvent += [] {
        openURL("https://github.com/adriengivry/Overload/issues/new?assignees=&labels=Feature&template=feature_request.md&title=");
    };
    helpMenu.createWidget<Separator>();
    helpMenu.createWidget<::vox::ui::Text>("Version: 1.3.0");
}

void MenuBar::updateToggleableItems() {
    
}

void MenuBar::openEveryWindows(bool p_state) {
    
}


}
}
}
