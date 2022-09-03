//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/widgets/input_fields/input_multiple_scalars.h"

namespace vox::ui {
/**
 * Input widget of multiple type float
 */
template <size_t Size>
class InputMultipleFloats : public InputMultipleScalars<float, Size> {
public:
    explicit InputMultipleFloats(float default_value = 0.0f,
                                 float step = 0.1f,
                                 float fast_step = 0.0f,
                                 const std::string &label = "",
                                 const std::string &format = "%.3f",
                                 bool select_all_on_click = true)
        : InputMultipleScalars<float, Size>(ImGuiDataType_::ImGuiDataType_Float,
                                            default_value,
                                            step,
                                            fast_step,
                                            label,
                                            format,
                                            select_all_on_click) {}
};

}  // namespace vox::ui
