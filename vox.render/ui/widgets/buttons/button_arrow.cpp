//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "button_arrow.h"

namespace vox {
namespace ui {
ButtonArrow::ButtonArrow(ImGuiDir p_direction) :
direction(p_direction) {
}

void ButtonArrow::_draw_Impl() {
    if (ImGui::ArrowButton(_widgetID.c_str(), direction))
        clickedEvent.invoke();
}

}
}
