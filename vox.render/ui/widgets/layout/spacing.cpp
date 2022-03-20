//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "spacing.h"

namespace vox {
namespace ui {
Spacing::Spacing(uint16_t p_spaces) :
spaces(p_spaces) {
}

void Spacing::_draw_Impl() {
    for (uint16_t i = 0; i < spaces; ++i) {
        ImGui::Spacing();
        
        if (i + 1 < spaces)
            ImGui::SameLine();
    }
}


}
}
