//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef input_multiple_ints_h
#define input_multiple_ints_h

#include "input_multiple_scalars.h"

namespace vox {
namespace ui {
/**
 * Input widget of multiple type int
 */
template<size_t _Size>
class InputMultipleInts : public InputMultipleScalars<int, _Size> {
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
    InputMultipleInts(int p_defaultValue = 0,
                      int p_step = 1,
                      int p_fastStep = 0,
                      const std::string &p_label = "",
                      const std::string &p_format = "%d",
                      bool p_selectAllOnClick = true) :
    InputMultipleScalars<int, _Size>(ImGuiDataType_::ImGuiDataType_S32, p_defaultValue,
                                     p_step, p_fastStep, p_label, p_format, p_selectAllOnClick) {
    }
};


}
}
#endif /* input_multiple_ints_h */
