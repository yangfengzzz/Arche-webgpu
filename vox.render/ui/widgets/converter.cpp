//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/converter.h"

namespace vox::ui {
ImVec4 Converter::ToImVec4(const Color &value) { return {value.r, value.g, value.b, value.a}; }

Color Converter::ToColor(const ImVec4 &value) { return {value.x, value.y, value.z, value.w}; }

ImVec2 Converter::ToImVec2(const Vector2F &value) { return {value.x, value.y}; }

Vector2F Converter::ToVector2F(const ImVec2 &value) { return {value.x, value.y}; }

}  // namespace vox::ui
