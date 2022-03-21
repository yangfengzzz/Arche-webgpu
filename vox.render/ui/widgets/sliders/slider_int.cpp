//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "slider_int.h"

namespace vox {
namespace ui {
SliderInt::SliderInt(int p_min,
                     int p_max,
                     int p_value,
                     SliderOrientation p_orientation,
                     const std::string &p_label,
                     const std::string &p_format) :
SliderSingleScalar<int>(ImGuiDataType_::ImGuiDataType_S32, p_min, p_max, p_value,
                        p_orientation, p_label, p_format) {
}


}
}
