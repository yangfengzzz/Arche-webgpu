//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "converter.h"

namespace vox {
namespace ui {
ImVec4 Converter::ToImVec4(const Color &p_value) {
    return ImVec4(p_value.r, p_value.g, p_value.b, p_value.a);
}

Color Converter::ToColor(const ImVec4 &p_value) {
    return Color(p_value.x, p_value.y, p_value.z, p_value.w);
}

ImVec2 Converter::ToImVec2(const Vector2F &p_value) {
    return ImVec2(p_value.x, p_value.y);
}

Vector2F Converter::ToVector2F(const ImVec2 &p_value) {
    return Vector2F(p_value.x, p_value.y);
}


}
}
