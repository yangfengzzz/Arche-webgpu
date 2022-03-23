//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "size_converter.h"
#include <math.h>

namespace vox {
namespace editor {
std::pair<float, SizeConverter::SizeUnit> SizeConverter::convertToOptimalUnit(float p_value, SizeUnit p_unit) {
    if (p_value == 0.0f) return {0.0f, SizeUnit::BYTE};
    const float bytes = convert(p_value, p_unit, SizeUnit::BYTE);
    const int digits = static_cast<int>(trunc(log10(bytes)));
    const SizeUnit targetUnit = static_cast<SizeUnit>(fmin(3.0f * floor(digits / 3.0f), static_cast<float>(SizeUnit::TERA_BYTE)));
    
    return {convert(bytes, SizeUnit::BYTE, targetUnit), targetUnit};
}

float SizeConverter::convert(float p_value, SizeUnit p_from, SizeUnit p_to) {
    const float fromValue = powf(1024.0f, static_cast<float>(p_from) / 3.0f);
    const float toValue = powf(1024.0f, static_cast<float>(p_to) / 3.0f);
    
    return p_value * (fromValue / toValue);
}

std::string SizeConverter::unitToString(SizeUnit p_unit) {
    switch (p_unit) {
        case SizeConverter::SizeUnit::BYTE:
            return "B";
        case SizeConverter::SizeUnit::KILO_BYTE:
            return "KB";
        case SizeConverter::SizeUnit::MEGA_BYTE:
            return "MB";
        case SizeConverter::SizeUnit::GIGA_BYTE:
            return "GB";
        case SizeConverter::SizeUnit::TERA_BYTE:
            return "TB";
    }
    
    return "?";
}

}
}
