//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/buttons/button_arrow.h"

namespace vox::ui {
ButtonArrow::ButtonArrow(ImGuiDir direction) : direction_(direction) {}

void ButtonArrow::DrawImpl() {
    if (ImGui::ArrowButton(widget_id_.c_str(), direction_)) clicked_event_.Invoke();
}

}  // namespace vox::ui
