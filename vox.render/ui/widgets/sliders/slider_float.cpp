//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/sliders/slider_float.h"

namespace vox::ui {
SliderFloat::SliderFloat(float min,
                         float max,
                         float value,
                         SliderOrientation orientation,
                         const std::string &label,
                         const std::string &format)
    : SliderSingleScalar<float>(ImGuiDataType_::ImGuiDataType_Float, min, max, value, orientation, label, format) {}

}  // namespace vox::ui
