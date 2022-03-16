//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "main_menu.h"
#include "gui/imgui.h"
#include "gui_entry.h"
#include "app_styles.h"

namespace vox {
namespace editor {
static void showWindowMenuItem(const char *title, bool *val) {
    ImGui::Checkbox(title, val);
}

MainMenu::MainMenu(GUIEntry *entry) {
    _entry = entry;
}

void MainMenu::showMainMenu() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            showFileMenu();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Options")) {
            showOptionsMenu();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Windows")) {
            showWindowsMenu();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
            showHelpMenu();
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void MainMenu::showFileMenu() {
    if (ImGui::MenuItem("Open")) {
    }

    if (ImGui::MenuItem("Save")) {
    }
}

void MainMenu::showOptionsMenu() {
    if (ImGui::BeginMenu("Themes")) {
        if (ImGui::MenuItem("Default")) {
            loadDefaultStyle();
        }

        if (ImGui::MenuItem("Maya Theme")) {
            loadMayaStyle();
        }

        if (ImGui::MenuItem("Black & White")) {
            loadBlackAndWhite();
        }

        if (ImGui::MenuItem("Cool Dark")) {
            loadDarkCoolStyle();
        }

        if (ImGui::MenuItem("Light Orange")) {
            loadLightOrngeStyle();
        }

        ImGui::EndMenu();
    }
}

void MainMenu::showWindowsMenu() {
    showWindowMenuItem("Statistics", &_entry->windows.statsWindow);
    showWindowMenuItem("Theme Editor", &_entry->windows.styleEditor);
//    showWindowMenuItem("Shader Editor", &appState->windows.shaderEditorWindow);
    showWindowMenuItem("Foliage Manager", &_entry->windows.foliageManager);
//    showWindowMenuItem("Texture Settings", &appState->windows.texturEditorWindow);
    showWindowMenuItem("Texture Store", &_entry->windows.textureStore);
    showWindowMenuItem("Sea Settings", &_entry->windows.seaEditor);
    showWindowMenuItem("Light Settings", &_entry->windows.lightControls);
    showWindowMenuItem("Camera Settings", &_entry->windows.cameraControls);
//    showWindowMenuItem("Mesh Generators Settings", &_entry->meshGenerator->windowStat);
    showWindowMenuItem("Sky Settings", &_entry->windows.skySettings);
    showWindowMenuItem("Filters Manager", &_entry->windows.filtersManager);
    showWindowMenuItem("Module Manager", &_entry->windows.modulesManager);
    showWindowMenuItem("Supporters", &_entry->windows.supportersTribute);
    showWindowMenuItem("Open Source Liscenses", &_entry->windows.osLisc);
}

void openURL(std::string url) {
    std::string op = std::string("open ").append(url);
    system(op.c_str());
}

void MainMenu::showHelpMenu() {
    if (ImGui::MenuItem("Social Handle")) {
        openURL("https://github.com/yangfengzzz");
    }

    if (ImGui::MenuItem("GitHub Page")) {
        openURL("https://github.com/ArcheGraphics");
    }

    if (ImGui::MenuItem("Documentation")) {
        openURL("https://arche.graphics/docs/intro");
    }
}

}
}
