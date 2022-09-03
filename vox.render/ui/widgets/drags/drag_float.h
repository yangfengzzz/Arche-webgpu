//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/widgets/drags/drag_single_scalar.h"

namespace vox::ui {
/**
 * Drag widget of type double
 */
class DragFloat : public DragSingleScalar<float> {
public:
    explicit DragFloat(float min = 0.0,
                       float max = 1.0,
                       float value = 0.5,
                       float speed = 0.1f,
                       const std::string &label = "",
                       const std::string &format = "%.3f");
};

}  // namespace vox::ui
