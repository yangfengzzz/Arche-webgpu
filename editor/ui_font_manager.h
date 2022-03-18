//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef ui_font_manager_hpp
#define ui_font_manager_hpp

#include "gui/imgui.h"
#include <string>

void loadUIFont(std::string name, float pixelSize, std::string path);

ImFont *getUIFont(std::string name);

#endif /* ui_font_manager_hpp */
