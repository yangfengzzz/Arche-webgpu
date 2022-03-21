//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef slider_double_hpp
#define slider_double_hpp

#include "slider_single_scalar.h"

namespace vox {
namespace ui {
/**
 * Slider widget of type double
 */
class SliderDouble : public SliderSingleScalar<double> {
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
    SliderDouble(double p_min = 0.0,
                 double p_max = 1.0,
                 double p_value = 0.5,
                 SliderOrientation p_orientation = SliderOrientation::HORIZONTAL,
                 const std::string &p_label = "",
                 const std::string &p_format = "%.6f");
};


}
}
#endif /* slider_double_hpp */
