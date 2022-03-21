//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "slider_float.h"

namespace vox {
namespace ui {
SliderFloat::SliderFloat(float p_min,
                         float p_max,
                         float p_value,
                         SliderOrientation p_orientation,
                         const std::string &p_label,
                         const std::string &p_format) :
SliderSingleScalar<float>(ImGuiDataType_::ImGuiDataType_Float, p_min, p_max,
                          p_value, p_orientation, p_label, p_format) {
}


}
}
