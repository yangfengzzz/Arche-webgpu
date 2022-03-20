//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef input_int_hpp
#define input_int_hpp

#include "input_single_scalar.h"

namespace vox {
namespace ui {
/**
 * Input widget of type int
 */
class InputInt : public InputSingleScalar<int> {
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
    InputInt(int p_defaultValue = 0,
             int p_step = 1,
             int p_fastStep = 0,
             const std::string &p_label = "",
             const std::string &p_format = "%d",
             bool p_selectAllOnClick = true);
};


}
}
#endif /* input_int_hpp */
