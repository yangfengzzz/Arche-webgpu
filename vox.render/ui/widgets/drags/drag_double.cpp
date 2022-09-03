//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/drags/drag_double.h"

namespace vox::ui {
DragDouble::DragDouble(
        double min, double max, double value, float speed, const std::string &label, const std::string &format)
    : DragSingleScalar<double>(ImGuiDataType_::ImGuiDataType_Double, min, max, value, speed, label, format) {}

}  // namespace vox::ui
