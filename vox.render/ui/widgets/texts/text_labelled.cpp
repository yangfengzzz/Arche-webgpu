//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "text_labelled.h"

namespace vox {
namespace ui {
TextLabelled::TextLabelled(const std::string &p_content, const std::string &p_label) :
Text(p_content), label(p_label) {
}

void TextLabelled::_draw_Impl() {
    ImGui::LabelText((label + _widgetID).c_str(), content.c_str());
}

}
}
