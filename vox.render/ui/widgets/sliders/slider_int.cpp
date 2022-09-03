//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/sliders/slider_int.h"

namespace vox::ui {
SliderInt::SliderInt(
        int min, int max, int value, SliderOrientation orientation, const std::string &label, const std::string &format)
    : SliderSingleScalar<int>(ImGuiDataType_::ImGuiDataType_S32, min, max, value, orientation, label, format) {}

}  // namespace vox::ui
