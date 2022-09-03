//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <imgui.h>

#include "vox.math/color.h"
#include "vox.math/vector2.h"
#include "vox.math/vector4.h"

namespace vox::ui {
/**
 * Handles imgui conversion to/from arche types
 */
class Converter {
public:
    /**
     * Convert the given Color to ImVec4
     */
    static ImVec4 ToImVec4(const Color &value);

    /**
     * Convert the given ImVec4 to Color
     */
    static Color ToColor(const ImVec4 &value);

    /**
     * Convert the given FVector2 to ImVec2
     */
    static ImVec2 ToImVec2(const Vector2F &value);

    /**
     * Convert the given ImVec2 to FVector2
     */
    static Vector2F ToVector2F(const ImVec2 &value);
};

}  // namespace vox::ui
