//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/selection/combo_box.h"

namespace vox::ui {
ComboBox::ComboBox(int current_choice) : DataWidget<int>(current_choice_), current_choice_(current_choice) {}

void ComboBox::DrawImpl() {
    if (choices_.find(current_choice_) == choices_.end()) current_choice_ = choices_.begin()->first;

    if (ImGui::BeginCombo(widget_id_.c_str(), choices_[current_choice_].c_str())) {
        for (const auto &[kEy, kValue] : choices_) {
            bool selected = kEy == current_choice_;

            if (ImGui::Selectable(kValue.c_str(), selected)) {
                if (!selected) {
                    ImGui::SetItemDefaultFocus();
                    current_choice_ = kEy;
                    value_changed_event_.Invoke(current_choice_);
                    NotifyChange();
                }
            }
        }

        ImGui::EndCombo();
    }
}

}  // namespace vox::ui
