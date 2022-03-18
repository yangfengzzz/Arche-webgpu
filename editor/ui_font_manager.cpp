//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "ui_font_manager.h"
#include <unordered_map>

static std::unordered_map<std::string, ImFont *> fonts;

void loadUIFont(std::string name, float pizelSize, std::string path) {
    ImGuiIO &io = ImGui::GetIO();
    fonts[name] = io.Fonts->AddFontFromFileTTF(path.c_str(), pizelSize);
}

ImFont *getUIFont(std::string name) {
    if (fonts.find(name) != fonts.end()) {
        return fonts[name];
    }

    return nullptr;
}
