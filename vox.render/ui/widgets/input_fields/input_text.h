//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/event.h"
#include "vox.render/ui/widgets/data_widget.h"

namespace vox::ui {
/**
 * Input widget of type string
 */
class InputText : public DataWidget<std::string> {
public:
    explicit InputText(std::string content = "", std::string label = "");

protected:
    void DrawImpl() override;

public:
    std::string content_;
    std::string label_;
    bool select_all_on_click_ = false;
    Event<std::string> content_changed_event_;
    Event<std::string> enter_pressed_event_;
};

}  // namespace vox::ui
