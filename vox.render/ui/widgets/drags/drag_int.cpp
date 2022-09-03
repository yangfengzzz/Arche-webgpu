//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/drags/drag_int.h"

namespace vox::ui {
DragInt::DragInt(int min, int max, int value, float speed, const std::string &label, const std::string &format)
    : DragSingleScalar<int>(ImGuiDataType_::ImGuiDataType_S32, min, max, value, speed, label, format) {}

}  // namespace vox::ui
