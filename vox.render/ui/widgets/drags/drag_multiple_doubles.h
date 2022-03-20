//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef drag_multiple_doubles_h
#define drag_multiple_doubles_h

#include "drag_multiple_scalars.h"

namespace vox {
namespace ui {
/**
 * Drag widget of multiple type int
 */
template<size_t _Size>
class DragMultipleDoubles : public DragMultipleScalars<int, _Size> {
public:
    /**
     * Constructor
     * @param p_min
     * @param p_max
     * @param p_value
     * @param p_speed
     * @param p_label
     * @param p_format
     */
    DragMultipleDoubles(double p_min = 0.0,
                        double p_max = 1.0,
                        double p_value = 0.5,
                        float p_speed = 0.1f,
                        const std::string &p_label = "",
                        const std::string &p_format = "%.6f")
    : DragMultipleScalars<double, _Size>(ImGuiDataType_::ImGuiDataType_Double,
                                         p_min, p_max, p_value, p_speed, p_label, p_format) {
    }
};


}
}
#endif /* drag_multiple_doubles_h */
