//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef input_multiple_doubles_h
#define input_multiple_doubles_h

#include "input_multiple_scalars.h"

namespace vox {
namespace ui {
/**
 * Input widget of multiple type double
 */
template<size_t _Size>
class InputMultipleDoubles : public InputMultipleScalars<double, _Size> {
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
    InputMultipleDoubles(double p_defaultValue = 0.0,
                         double p_step = 0.1,
                         double p_fastStep = 0.0,
                         const std::string &p_label = "",
                         const std::string &p_format = "%.6f",
                         bool p_selectAllOnClick = true) :
    InputMultipleScalars<double, _Size>(ImGuiDataType_::ImGuiDataType_Double,
                                        p_defaultValue, p_step, p_fastStep, p_label, p_format, p_selectAllOnClick) {
    }
};

}
}
#endif /* input_multiple_doubles_h */
