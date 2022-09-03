//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/selection/color_edit.h"

namespace vox::ui {
ColorEdit::ColorEdit(bool enable_alpha, const Color &default_color)
    : DataWidget<Color>(color_), enable_alpha_(enable_alpha), color_(default_color) {}

void ColorEdit::DrawImpl() {
    int flags = !enable_alpha_ ? ImGuiColorEditFlags_NoAlpha : 0;
    bool value_changed = false;

    if (enable_alpha_)
        value_changed = ImGui::ColorEdit4(widget_id_.c_str(), &color_.r, flags);
    else
        value_changed = ImGui::ColorEdit3(widget_id_.c_str(), &color_.r, flags);

    if (value_changed) {
        color_changed_event_.Invoke(color_);
        NotifyChange();
    }
}

}  // namespace vox::ui
