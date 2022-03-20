//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef input_multiple_floats_h
#define input_multiple_floats_h

#include "input_multiple_scalars.h"

namespace vox {
namespace ui {
/**
 * Input widget of multiple type float
 */
template<size_t _Size>
class InputMultipleFloats : public InputMultipleScalars<float, _Size> {
public:
    /**
     * Constructor
     * @param p_defaultValue
     * @param p_step
     * @param p_fastStep
     * @param p_label
     * @param p_format
     * @param p_selectAllOnClick
     */
    InputMultipleFloats(float p_defaultValue = 0.0f,
                        float p_step = 0.1f,
                        float p_fastStep = 0.0f,
                        const std::string &p_label = "",
                        const std::string &p_format = "%.3f",
                        bool p_selectAllOnClick = true) :
    InputMultipleScalars<float, _Size>(ImGuiDataType_::ImGuiDataType_Float,
                                       p_defaultValue, p_step, p_fastStep, p_label, p_format, p_selectAllOnClick) {
    }
};

}
}
#endif /* input_multiple_floats_h */
