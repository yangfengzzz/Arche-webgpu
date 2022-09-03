//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/widgets/drags/drag_single_scalar.h"

namespace vox::ui {
/**
 * Drag widget of type int
 */
class DragInt : public DragSingleScalar<int> {
public:
    explicit DragInt(int min = 0,
                     int max = 100,
                     int value = 50,
                     float speed = 0.1f,
                     const std::string &label = "",
                     const std::string &format = "%d");
};

}  // namespace vox::ui
