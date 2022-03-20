//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "drag_int.h"

namespace vox {
namespace ui {
DragInt::DragInt(int p_min,
                 int p_max,
                 int p_value,
                 float p_speed,
                 const std::string &p_label,
                 const std::string &p_format) :
DragSingleScalar<int>(ImGuiDataType_::ImGuiDataType_S32, p_min, p_max,
                      p_value, p_speed, p_label, p_format) {
}


}
}
