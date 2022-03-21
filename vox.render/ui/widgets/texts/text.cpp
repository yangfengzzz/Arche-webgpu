//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "text.h"

namespace vox {
namespace ui {
Text::Text(const std::string &p_content) :
DataWidget(content),
content(p_content) {
}

void Text::_draw_Impl() {
    ImGui::Text(content.c_str());
}

}
}
