//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/widgets/sliders/slider_single_scalar.h"

namespace vox::ui {
/**
 * Slider widget of type double
 */
class SliderDouble : public SliderSingleScalar<double> {
public:
    explicit SliderDouble(double min = 0.0,
                          double max = 1.0,
                          double value = 0.5,
                          SliderOrientation orientation = SliderOrientation::HORIZONTAL,
                          const std::string &label = "",
                          const std::string &format = "%.6f");
};

}  // namespace vox::ui
