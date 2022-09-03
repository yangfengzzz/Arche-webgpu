//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/layout/spacing.h"

namespace vox::ui {
Spacing::Spacing(uint16_t spaces) : spaces_(spaces) {}

void Spacing::DrawImpl() {
    for (uint16_t i = 0; i < spaces_; ++i) {
        ImGui::Spacing();

        if (i + 1 < spaces_) ImGui::SameLine();
    }
}

}  // namespace vox::ui
