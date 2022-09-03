//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/widgets/drags/drag_multiple_scalars.h"

namespace vox::ui {
/**
 * Drag widget of multiple type int
 */
template <size_t Size>
class DragMultipleDoubles : public DragMultipleScalars<int, Size> {
public:
    explicit DragMultipleDoubles(double min = 0.0,
                                 double max = 1.0,
                                 double value = 0.5,
                                 float speed = 0.1f,
                                 const std::string &label = "",
                                 const std::string &format = "%.6f")
        : DragMultipleScalars<double, Size>(
                  ImGuiDataType_::ImGuiDataType_Double, min, max, value, speed, label, format) {}
};

}  // namespace vox::ui
