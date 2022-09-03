//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/buttons/button_simple.h"

#include <imgui_internal.h>

#include <utility>

#include "vox.render/ui/widgets/converter.h"

namespace vox::ui {
ButtonSimple::ButtonSimple(std::string label, const Vector2F &size, bool disabled)
    : label_(std::move(label)), size_(size), disabled_(disabled) {
    auto &style = ImGui::GetStyle();

    idle_background_color_ = Converter::ToColor(style.Colors[ImGuiCol_Button]);
    hovered_background_color_ = Converter::ToColor(style.Colors[ImGuiCol_ButtonHovered]);
    clicked_background_color_ = Converter::ToColor(style.Colors[ImGuiCol_ButtonActive]);
    text_color_ = Converter::ToColor(style.Colors[ImGuiCol_Text]);
}

void ButtonSimple::DrawImpl() {
    auto &style = ImGui::GetStyle();

    auto default_idle_color = style.Colors[ImGuiCol_Button];
    auto default_hovered_color = style.Colors[ImGuiCol_ButtonHovered];
    auto default_clicked_color = style.Colors[ImGuiCol_ButtonActive];
    auto default_text_color = style.Colors[ImGuiCol_Text];

    style.Colors[ImGuiCol_Button] = Converter::ToImVec4(idle_background_color_);
    style.Colors[ImGuiCol_ButtonHovered] = Converter::ToImVec4(hovered_background_color_);
    style.Colors[ImGuiCol_ButtonActive] = Converter::ToImVec4(clicked_background_color_);
    style.Colors[ImGuiCol_Text] = Converter::ToImVec4(text_color_);

    if (ImGui::ButtonEx((label_ + widget_id_).c_str(), Converter::ToImVec2(size_),
                        disabled_ ? ImGuiItemFlags_Disabled : 0))
        clicked_event_.Invoke();

    style.Colors[ImGuiCol_Button] = default_idle_color;
    style.Colors[ImGuiCol_ButtonHovered] = default_hovered_color;
    style.Colors[ImGuiCol_ButtonActive] = default_clicked_color;
    style.Colors[ImGuiCol_Text] = default_text_color;
}

}  // namespace vox::ui
