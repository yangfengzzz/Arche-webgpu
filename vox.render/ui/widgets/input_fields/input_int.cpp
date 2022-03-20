//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "input_int.h"

namespace vox {
namespace ui {
InputInt::InputInt(int p_defaultValue, int p_step, int p_fastStep,
                   const std::string &p_label, const std::string &p_format, bool p_selectAllOnClick)
: InputSingleScalar<int>(ImGuiDataType_::ImGuiDataType_S32, p_defaultValue,
                         p_step, p_fastStep, p_label, p_format, p_selectAllOnClick) {
}

}
}
