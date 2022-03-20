//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "drag_double.h"

namespace vox {
namespace ui {
DragDouble::DragDouble(double p_min,
                       double p_max,
                       double p_value,
                       float p_speed,
                       const std::string &p_label,
                       const std::string &p_format) :
DragSingleScalar<double>(ImGuiDataType_::ImGuiDataType_Double, p_min, p_max,
                         p_value, p_speed, p_label, p_format) {
}


}
}
