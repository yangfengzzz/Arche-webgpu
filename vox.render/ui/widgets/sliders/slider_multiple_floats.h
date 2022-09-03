//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/widgets/sliders/slider_multiple_scalars.h"

namespace vox::ui {
/**
 * Slider widget of multiple type float
 */
template <size_t Size>
class SliderMultipleFloats : public SliderMultipleScalars<float, Size> {
public:
    explicit SliderMultipleFloats(float min = 0.0f,
                                  float max = 1.0f,
                                  float value = 0.5f,
                                  const std::string &label = "",
                                  const std::string &format = "%.3f")
        : SliderMultipleScalars<float, Size>(ImGuiDataType_::ImGuiDataType_Float, min, max, value, label, format) {}
};

}  // namespace vox::ui
