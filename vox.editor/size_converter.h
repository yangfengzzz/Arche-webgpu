//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cstdint>
#include <string>
#include <tuple>

namespace vox {
namespace editor {
/*
 * Handles size conversions
 */
class SizeConverter {
public:
    enum class SizeUnit { BYTE = 0, KILO_BYTE = 3, MEGA_BYTE = 6, GIGA_BYTE = 9, TERA_BYTE = 12 };

    /**
     * Disabled constructor
     */
    SizeConverter() = delete;

    /**
     * Converts the given size to the optimal unit to avoid large numbers (Ex: 1000B will returns 1KB)
     */
    static std::pair<float, SizeUnit> convertToOptimalUnit(float p_value, SizeUnit p_unit);

    /**
     * Converts the given size from one unit to another
     */
    static float convert(float p_value, SizeUnit p_from, SizeUnit p_to);

    /**
     * Converts the given unit to a string
     */
    static std::string unitToString(SizeUnit p_unit);
};

}  // namespace editor
}  // namespace vox
