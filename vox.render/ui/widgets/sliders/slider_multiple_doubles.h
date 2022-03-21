//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef slider_multiple_doubles_h
#define slider_multiple_doubles_h

#include "ui/widgets/sliders/slider_multiple_scalars.h"

namespace vox {
namespace ui {
/**
 * Slider widget of multiple type double
 */
template<size_t _Size>
class SliderMultipleDoubles : public SliderMultipleScalars<double, _Size> {
public:
    /**
     * Constructor
     * @param p_min
     * @param p_max
     * @param p_value
     * @param p_label
     * @param p_format
     */
    SliderMultipleDoubles(double p_min = 0.0,
                          double p_max = 1.0,
                          double p_value = 0.5,
                          const std::string &p_label = "",
                          const std::string &p_format = "%.6f") :
    SliderMultipleScalars<double, _Size>(ImGuiDataType_::ImGuiDataType_Double,
                                         p_min, p_max, p_value, p_label, p_format) {
    }
};

}
}
#endif /* slider_multiple_doubles_h */
