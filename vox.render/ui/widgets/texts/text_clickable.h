//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/event.h"
#include "vox.render/ui/widgets/texts/text.h"

namespace vox::ui {
/**
 * Widget to display text on a panel that is also clickable
 */
class TextClickable : public Text {
public:
    explicit TextClickable(const std::string &content = "");

protected:
    void DrawImpl() override;

public:
    Event<> clicked_event_;
    Event<> double_clicked_event_;
};

}  // namespace vox::ui
