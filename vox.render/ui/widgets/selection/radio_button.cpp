//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/selection/radio_button.h"

#include <utility>

namespace vox::ui {
RadioButton::RadioButton(bool selected, std::string label) : DataWidget<bool>(selected_), label_(std::move(label)) {
    if (selected) Select();
}

void RadioButton::Select() {
    selected_ = true;
    clicked_event_.Invoke(radio_id_);
}

bool RadioButton::IsSelected() const { return selected_; }

bool RadioButton::RadioId() const { return radio_id_; }

void RadioButton::DrawImpl() {
    if (ImGui::RadioButton((label_ + widget_id_).c_str(), selected_)) {
        clicked_event_.Invoke(radio_id_);
        NotifyChange();
    }
}

}  // namespace vox::ui
