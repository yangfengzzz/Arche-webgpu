//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "text_wrapped.h"

namespace vox {
namespace ui {
TextWrapped::TextWrapped(const std::string &p_content) :
Text(p_content) {
}

void TextWrapped::_draw_Impl() {
    ImGui::TextWrapped(content.c_str());
}


}
}
