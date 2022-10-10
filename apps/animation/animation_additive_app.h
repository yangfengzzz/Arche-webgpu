//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/forward_application.h"
#include "vox.render/ui/canvas.h"
#include "vox.render/ui/widgets/panel_transformables/panel_window.h"

namespace vox {
class AnimationAdditiveApp : public ForwardApplication {
public:
    void loadScene() override;

private:
    ui::Canvas canvas_;
    ui::PanelWindow panel_;
};
}  // namespace vox