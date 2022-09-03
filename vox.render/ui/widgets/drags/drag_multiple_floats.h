//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/widgets/drags/drag_multiple_scalars.h"

namespace vox::ui {
/**
 * Drag widget of multiple type float
 */
template <size_t Size>
class DragMultipleFloats : public DragMultipleScalars<int, Size> {
public:
    explicit DragMultipleFloats(float min = 0.0f,
                                float max = 1.0f,
                                float value = 0.5f,
                                float speed = 0.1f,
                                const std::string &label = "",
                                const std::string &format = "%.3f")
        : DragMultipleScalars<float, Size>(ImGuiDataType_::ImGuiDataType_Float, min, max, value, speed, label, format) {
    }
};

}  // namespace vox::ui
