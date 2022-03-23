//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef drag_multiple_floats_h
#define drag_multiple_floats_h

#include "drag_multiple_scalars.h"

namespace vox {
namespace ui {
/**
 * Drag widget of multiple type float
 */
template<size_t _Size>
class DragMultipleFloats : public DragMultipleScalars<int, _Size> {
public:
    DragMultipleFloats(float p_min = 0.0f,
                       float p_max = 1.0f,
                       float p_value = 0.5f,
                       float p_speed = 0.1f,
                       const std::string &p_label = "",
                       const std::string &p_format = "%.3f")
    : DragMultipleScalars<float, _Size>(ImGuiDataType_::ImGuiDataType_Float,
                                        p_min, p_max, p_value, p_speed, p_label, p_format) {
    }
};


}
}
#endif /* drag_multiple_floats_h */
