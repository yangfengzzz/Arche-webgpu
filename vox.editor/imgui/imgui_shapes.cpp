//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "imgui_shapes.h"
#define IMGUI_DEFINE_MATH_OPERATORS

#include <iostream>

#include "gui/imgui_internal.h"

void ImGui::DrawCircle(float radius, ImU32 color, float segments, float thickness) {
    ImDrawList *list = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorPos() + ImGui::GetWindowPos();
    list->AddCircle(ImVec2(pos.x + radius, pos.y + radius), radius, color, segments, thickness);
    // ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(2*radius , 2*radius));
}

void ImGui::DrawFilledCircle(float radius, ImU32 color, float segments) {
    ImDrawList *list = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorPos() + ImGui::GetWindowPos();
    list->AddCircleFilled(ImVec2(pos.x + radius, pos.y + radius), radius, color, segments);
    // ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(2*radius , 2*radius));
}

void ImGui::DrawRect(ImVec2 size, ImU32 color, float rounding, float thickness) {
    ImDrawList *list = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorPos() + ImGui::GetWindowPos();
    list->AddRect(pos, pos + size, color, rounding, 0, thickness);
    // ImGui::SetCursorPos(ImGui::GetCursorPos() + size);
}

void ImGui::DrawFilledRect(ImVec2 size, ImU32 color, float rounding) {
    ImDrawList *list = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorPos() + ImGui::GetWindowPos();
    list->AddRectFilled(pos, pos + size, color, rounding, 0);
    // ImGui::SetCursorPos(ImGui::GetCursorPos() + size);
}
