//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef imgui_shapes_hpp
#define imgui_shapes_hpp

#include <imgui.h>

namespace ImGui {

void DrawCircle(float radius = 5, ImU32 color = ImColor(255, 255, 255), float segments = 0, float thickness = 1.0);

void DrawFilledCircle(float radius = 5, ImU32 color = ImColor(255, 255, 255), float segments = 0);

void DrawRect(ImVec2 size = ImVec2(10, 10),
              ImU32 color = ImColor(255, 255, 255),
              float rounding = 0.0f,
              float thickness = 1.0f);

void DrawFilledRect(ImVec2 size = ImVec2(10, 10), ImU32 color = ImColor(255, 255, 255), float rounding = 0.0f);

}  // namespace ImGui
#endif /* imgui_shapes_hpp */
