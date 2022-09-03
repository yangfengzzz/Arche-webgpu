//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/input_fields/input_double.h"

namespace vox::ui {
InputDouble::InputDouble(double default_value,
                         double step,
                         double fast_step,
                         const std::string &label,
                         const std::string &format,
                         bool select_all_on_click)
    : InputSingleScalar<double>(ImGuiDataType_::ImGuiDataType_Double,
                                default_value,
                                step,
                                fast_step,
                                label,
                                format,
                                select_all_on_click) {}

}  // namespace vox::ui
