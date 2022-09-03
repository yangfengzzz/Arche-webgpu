//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/widgets/buttons/button.h"

namespace vox::ui {
/**
 * Button widget with an arrow image
 */
class ButtonArrow : public Button {
public:
    /**
     * Create the button
     */
    explicit ButtonArrow(ImGuiDir direction = ImGuiDir_None);

protected:
    void DrawImpl() override;

public:
    ImGuiDir direction_;
};

}  // namespace vox::ui
