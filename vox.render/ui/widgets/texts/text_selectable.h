//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/widgets/texts/text.h"

namespace vox::ui {
/**
 * Simple widget to display a selectable text on a panel
 */
class TextSelectable : public Text {
public:
    explicit TextSelectable(const std::string &content = "", bool selected = false, bool disabled = false);

protected:
    void DrawImpl() override;

public:
    bool selected_;
    bool disabled_;

    Event<bool> clicked_event_;
    Event<> selected_event_;
    Event<> unselected_event_;
};

}  // namespace vox::ui
