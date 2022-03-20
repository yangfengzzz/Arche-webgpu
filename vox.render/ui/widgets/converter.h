//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef converter_hpp
#define converter_hpp

#include "gui/imgui.h"
#include "vector2.h"
#include "vector4.h"
#include "color.h"

namespace vox {
namespace ui {
/**
 * Handles imgui conversion to/from arche types
 */
class Converter {
public:
    /**
     * Convert the given Color to ImVec4
     */
    static ImVec4 ToImVec4(const Color &p_value);
    
    /**
     * Convert the given ImVec4 to Color
     */
    static Color ToColor(const ImVec4 &p_value);
    
    /**
     * Convert the given FVector2 to ImVec2
     */
    static ImVec2 ToImVec2(const Vector2F &p_value);
    
    /**
     * Convert the given ImVec2 to FVector2
     */
    static Vector2F ToVector2F(const ImVec2 &p_value);
};


}
}
#endif /* converter_hpp */
