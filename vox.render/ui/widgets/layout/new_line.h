//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/widgets/widget.h"

namespace vox::ui {
/**
 * Widget that adds an empty line to the panel
 */
class NewLine : public Widget {
protected:
    void DrawImpl() override;
};

}  // namespace vox::ui
