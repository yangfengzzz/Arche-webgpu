//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/widgets/widget.h"

namespace vox::ui {
/**
 * Widget that adds a space to the panel line
 */
class Spacing : public Widget {
public:
    explicit Spacing(uint16_t spaces = 1);

protected:
    void DrawImpl() override;

public:
    uint16_t spaces_ = 1;
};

}  // namespace vox::ui
