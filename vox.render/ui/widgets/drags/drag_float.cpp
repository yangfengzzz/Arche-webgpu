//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "drag_float.h"

namespace vox {
namespace ui {
DragFloat::DragFloat(float p_min,
                     float p_max,
                     float p_value,
                     float p_speed,
                     const std::string &p_label,
                     const std::string &p_format) :
DragSingleScalar<float>(ImGuiDataType_::ImGuiDataType_Float, p_min, p_max,
                        p_value, p_speed, p_label, p_format) {
}


}
}
