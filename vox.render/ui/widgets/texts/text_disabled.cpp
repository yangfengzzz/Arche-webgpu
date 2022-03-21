//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "text_disabled.h"

namespace vox {
namespace ui {
TextDisabled::TextDisabled(const std::string &p_content) :
Text(p_content) {
}

void TextDisabled::_draw_Impl() {
    ImGui::TextDisabled(content.c_str());
}

}
}
