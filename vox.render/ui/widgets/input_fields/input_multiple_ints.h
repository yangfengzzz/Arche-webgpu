//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/widgets/input_fields/input_multiple_scalars.h"

namespace vox::ui {
/**
 * Input widget of multiple type int
 */
template <size_t Size>
class InputMultipleInts : public InputMultipleScalars<int, Size> {
public:
    explicit InputMultipleInts(int default_value = 0,
                               int step = 1,
                               int fast_step = 0,
                               const std::string &label = "",
                               const std::string &format = "%d",
                               bool select_all_on_click = true)
        : InputMultipleScalars<int, Size>(ImGuiDataType_::ImGuiDataType_S32,
                                          default_value,
                                          step,
                                          fast_step,
                                          label,
                                          format,
                                          select_all_on_click) {}
};

}  // namespace vox::ui
