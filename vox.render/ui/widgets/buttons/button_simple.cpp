//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "button_simple.h"
#include "ui/widgets/converter.h"
#include "gui/imgui_internal.h"

namespace vox {
namespace ui {
ButtonSimple::ButtonSimple(const std::string &p_label, const Vector2F &p_size, bool p_disabled) :
label(p_label), size(p_size), disabled(p_disabled) {
    auto &style = ImGui::GetStyle();
    
    idleBackgroundColor = Converter::ToColor(style.Colors[ImGuiCol_Button]);
    hoveredBackgroundColor = Converter::ToColor(style.Colors[ImGuiCol_ButtonHovered]);
    clickedBackgroundColor = Converter::ToColor(style.Colors[ImGuiCol_ButtonActive]);
    textColor = Converter::ToColor(style.Colors[ImGuiCol_Text]);
}

void ButtonSimple::_draw_Impl() {
    auto &style = ImGui::GetStyle();
    
    auto defaultIdleColor = style.Colors[ImGuiCol_Button];
    auto defaultHoveredColor = style.Colors[ImGuiCol_ButtonHovered];
    auto defaultClickedColor = style.Colors[ImGuiCol_ButtonActive];
    auto defaultTextColor = style.Colors[ImGuiCol_Text];
    
    style.Colors[ImGuiCol_Button] = Converter::ToImVec4(idleBackgroundColor);
    style.Colors[ImGuiCol_ButtonHovered] = Converter::ToImVec4(hoveredBackgroundColor);
    style.Colors[ImGuiCol_ButtonActive] = Converter::ToImVec4(clickedBackgroundColor);
    style.Colors[ImGuiCol_Text] = Converter::ToImVec4(textColor);
    
    if (ImGui::ButtonEx((label + _widgetID).c_str(), Converter::ToImVec2(size), disabled ? ImGuiItemFlags_Disabled : 0))
        ClickedEvent.invoke();
    
    style.Colors[ImGuiCol_Button] = defaultIdleColor;
    style.Colors[ImGuiCol_ButtonHovered] = defaultHoveredColor;
    style.Colors[ImGuiCol_ButtonActive] = defaultClickedColor;
    style.Colors[ImGuiCol_Text] = defaultTextColor;
}


}
}
