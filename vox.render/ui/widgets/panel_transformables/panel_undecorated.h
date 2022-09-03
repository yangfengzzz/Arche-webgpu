//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/widgets/panel_transformable.h"

namespace vox::ui {
/**
 * A simple panel that is transformable and without decorations (No background)
 */
class PanelUndecorated : public PanelTransformable {
public:
    void DrawImpl() override;

private:
    int CollectFlags();
};

}  // namespace vox::ui
