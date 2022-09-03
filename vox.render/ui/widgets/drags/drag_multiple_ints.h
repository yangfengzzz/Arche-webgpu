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
class DragMultipleInts : public DragMultipleScalars<int, Size> {
public:
    explicit DragMultipleInts(int min = 0,
                              int max = 100,
                              int value = 50,
                              float speed = 1.0f,
                              const std::string &label = "",
                              const std::string &format = "%d")
        : DragMultipleScalars<int, Size>(ImGuiDataType_::ImGuiDataType_S32, min, max, value, speed, label, format) {}
};

}  // namespace vox::ui
