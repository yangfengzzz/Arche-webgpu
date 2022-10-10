//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef imgui_curve_hpp
#define imgui_curve_hpp

#include <imgui.h>

namespace ImGui {

int Curve(const char *label, const ImVec2 &size, int maxpoints, ImVec2 *points);

float CurveValue(float p, int maxpoints, const ImVec2 *points);

float CurveValueSmooth(float p, int maxpoints, const ImVec2 *points);

};     // namespace ImGui
#endif /* imgui_curve_hpp */
