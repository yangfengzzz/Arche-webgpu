//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/widgets/sliders/slider_multiple_scalars.h"

namespace vox::ui {
/**
 * Slider widget of multiple type int
 */
template <size_t Size>
class SliderMultipleInts : public SliderMultipleScalars<int, Size> {
public:
    explicit SliderMultipleInts(
            int min = 0, int max = 100, int value = 50, const std::string &label = "", const std::string &format = "%d")
        : SliderMultipleScalars<int, Size>(ImGuiDataType_::ImGuiDataType_S32, min, max, value, label, format) {}
};

}  // namespace vox::ui
