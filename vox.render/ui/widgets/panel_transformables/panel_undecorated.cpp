//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/panel_transformables/panel_undecorated.h"

namespace vox::ui {
void PanelUndecorated::DrawImpl() {
    auto &style = ImGui::GetStyle();
    ImVec2 previous_padding = style.WindowPadding;
    ImVec2 previous_min_size = style.WindowMinSize;
    style.WindowPadding = {0, 0};
    style.WindowMinSize = {0, 0};

    if (ImGui::Begin(panel_id_.c_str(), nullptr, CollectFlags())) {
        style.WindowPadding = previous_padding;
        style.WindowMinSize = previous_min_size;

        Update();

        DrawWidgets();

        ImGui::End();
    } else {
        style.WindowPadding = previous_padding;
    }
}

int PanelUndecorated::CollectFlags() {
    ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                             ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground;

    if (auto_size_) flags |= ImGuiWindowFlags_AlwaysAutoResize;

    return flags;
}

}  // namespace vox::ui
