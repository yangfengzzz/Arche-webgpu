//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/color.h"
#include "vox.render/event.h"
#include "vox.render/ui/widgets/data_widget.h"

namespace vox::ui {
/**
 * Widget that allow the selection of a color with a color picker
 */
class ColorPicker : public DataWidget<Color> {
public:
    explicit ColorPicker(bool enable_alpha = false, const Color &default_color = {});

protected:
    void DrawImpl() override;

public:
    bool enable_alpha_;
    Color color_;
    Event<Color &> color_changed_event_;
};

}  // namespace vox::ui
