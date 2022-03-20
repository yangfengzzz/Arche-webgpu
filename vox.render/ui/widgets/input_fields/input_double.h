//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef input_double_hpp
#define input_double_hpp

#include "input_single_scalar.h"

namespace vox {
namespace ui {
/**
 * Input widget of type double
 */
class InputDouble : public InputSingleScalar<double> {
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
    InputDouble(double p_defaultValue = 0.0,
                double p_step = 0.1,
                double p_fastStep = 0.0,
                const std::string &p_label = "",
                const std::string &p_format = "%.6f",
                bool p_selectAllOnClick = true);
};

}
}
#endif /* input_double_hpp */
