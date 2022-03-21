//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef slider_float_hpp
#define slider_float_hpp

#include "slider_single_scalar.h"

namespace vox {
namespace ui {
/**
 * Slider widget of type float
 */
class SliderFloat : public SliderSingleScalar<float> {
public:
    /**
     * Constructor
     * @param p_min p_min
     * @param p_max p_max
     * @param p_value p_value
     * @param p_orientation p_orientation
     * @param p_label p_label
     * @param p_format p_format
     */
    SliderFloat(float p_min = 0.0f,
                float p_max = 1.0f,
                float p_value = 0.5f,
                SliderOrientation p_orientation = SliderOrientation::HORIZONTAL,
                const std::string &p_label = "",
                const std::string &p_format = "%.3f");
};


}
}
#endif /* slider_float_hpp */
