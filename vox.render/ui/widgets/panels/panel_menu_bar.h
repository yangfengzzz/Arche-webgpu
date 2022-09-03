//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/widgets/panel.h"

namespace vox::ui {
/**
 * A simple panel that will be displayed on the top side of the canvas
 */
class PanelMenuBar : public Panel {
protected:
    void DrawImpl() override;
};

}  // namespace vox::ui
