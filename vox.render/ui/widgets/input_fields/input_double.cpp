//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "input_double.h"

namespace vox {
namespace ui {
InputDouble::InputDouble(double p_defaultValue, double p_step, double p_fastStep,
                         const std::string &p_label, const std::string &p_format,
                         bool p_selectAllOnClick) :
InputSingleScalar<double>(ImGuiDataType_::ImGuiDataType_Double,
                          p_defaultValue, p_step, p_fastStep,
                          p_label, p_format, p_selectAllOnClick) {
}

}
}
