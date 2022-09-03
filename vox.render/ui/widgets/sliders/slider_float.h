//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/widgets/sliders/slider_single_scalar.h"

namespace vox::ui {
/**
 * Slider widget of type float
 */
class SliderFloat : public SliderSingleScalar<float> {
public:
    explicit SliderFloat(float min = 0.0f,
                         float max = 1.0f,
                         float value = 0.5f,
                         SliderOrientation orientation = SliderOrientation::HORIZONTAL,
                         const std::string &label = "",
                         const std::string &format = "%.3f");
};

}  // namespace vox::ui
