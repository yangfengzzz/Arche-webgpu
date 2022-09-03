//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/widgets/sliders/slider_single_scalar.h"

namespace vox::ui {
/**
 * Slider widget of type int
 */
class SliderInt : public SliderSingleScalar<int> {
public:
    explicit SliderInt(int min = 0,
                       int max = 100,
                       int value = 50,
                       SliderOrientation orientation = SliderOrientation::HORIZONTAL,
                       const std::string &label = "",
                       const std::string &format = "%d");
};

}  // namespace vox::ui
