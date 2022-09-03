//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/texts/text_clickable.h"

namespace vox::ui {
TextClickable::TextClickable(const std::string &content) : Text(content) {}

void TextClickable::DrawImpl() {
    bool useless = false;

    if (ImGui::Selectable((content_ + widget_id_).c_str(), &useless, ImGuiSelectableFlags_AllowDoubleClick)) {
        if (ImGui::IsMouseDoubleClicked(0)) {
            double_clicked_event_.Invoke();
        } else {
            clicked_event_.Invoke();
        }
    }
}

}  // namespace vox::ui
