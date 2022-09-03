//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/color.h"
#include "vox.math/vector2.h"
#include "vox.render/ui/widgets/buttons/button.h"

namespace vox::ui {
/**
 * Small button widget
 */
class ButtonSmall : public Button {
public:
    /**
     * Constructor
     */
    explicit ButtonSmall(std::string label = "");

protected:
    void DrawImpl() override;

public:
    std::string label_;

    Color idle_background_color_;
    Color hovered_background_color_;
    Color clicked_background_color_;

    Color text_color_;
};

}  // namespace vox::ui
