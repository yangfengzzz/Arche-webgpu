//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "panel_undecorated.h"

namespace vox {
namespace ui {
void PanelUndecorated::_draw_Impl() {
    auto &style = ImGui::GetStyle();
    ImVec2 previousPadding = style.WindowPadding;
    ImVec2 previousMinSize = style.WindowMinSize;
    style.WindowPadding = {0, 0};
    style.WindowMinSize = {0, 0};
    
    if (ImGui::Begin(_panelID.c_str(), nullptr, collectFlags())) {
        style.WindowPadding = previousPadding;
        style.WindowMinSize = previousMinSize;
        
        update();
        
        drawWidgets();
        
        ImGui::End();
    } else {
        style.WindowPadding = previousPadding;
    }
}

int PanelUndecorated::collectFlags() {
    ImGuiWindowFlags flags =
    ImGuiWindowFlags_AlwaysUseWindowPadding |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoScrollWithMouse |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoBackground;
    
    if (autoSize) flags |= ImGuiWindowFlags_AlwaysAutoResize;
    
    return flags;
}


}
}
