//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "input_float.h"

namespace vox {
namespace ui {
InputFloat::InputFloat(float p_defaultValue, float p_step, float p_fastStep,
                       const std::string &p_label, const std::string &p_format,
                       bool p_selectAllOnClick)
: InputSingleScalar<float>(ImGuiDataType_::ImGuiDataType_Float,
                           p_defaultValue, p_step, p_fastStep,
                           p_label, p_format, p_selectAllOnClick) {
}

}
}
