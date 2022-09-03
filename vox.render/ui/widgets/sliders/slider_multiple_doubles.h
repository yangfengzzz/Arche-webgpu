//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/widgets/sliders/slider_multiple_scalars.h"

namespace vox::ui {
/**
 * Slider widget of multiple type double
 */
template <size_t Size>
class SliderMultipleDoubles : public SliderMultipleScalars<double, Size> {
public:
    explicit SliderMultipleDoubles(double min = 0.0,
                                   double max = 1.0,
                                   double value = 0.5,
                                   const std::string &label = "",
                                   const std::string &format = "%.6f")
        : SliderMultipleScalars<double, Size>(ImGuiDataType_::ImGuiDataType_Double, min, max, value, label, format) {}
};

}  // namespace vox::ui
