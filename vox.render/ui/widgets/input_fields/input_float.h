//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef input_float_hpp
#define input_float_hpp

#include "input_single_scalar.h"

namespace vox {
namespace ui {
/**
 * Input widget of type float
 */
class InputFloat : public InputSingleScalar<float> {
public:
    /**
     * Constructor
     * @param p_defaultValue p_defaultValue
     * @param p_step p_step
     * @param p_fastStep p_fastStep
     * @param p_label p_label
     * @param p_format p_format
     * @param p_selectAllOnClick p_selectAllOnClick
     */
    InputFloat(float p_defaultValue = 0.0f,
               float p_step = 0.1f,
               float p_fastStep = 0.0f,
               const std::string &p_label = "",
               const std::string &p_format = "%.3f",
               bool p_selectAllOnClick = true);
};


}
}
#endif /* input_float_hpp */
