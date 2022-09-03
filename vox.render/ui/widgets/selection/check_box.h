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
 * Checkbox widget that can be checked or not
 */
class CheckBox : public DataWidget<bool> {
public:
    explicit CheckBox(bool value = false, std::string label = "");

protected:
    void DrawImpl() override;

public:
    bool value_;
    std::string label_;
    Event<bool> value_changed_event_;
};

}  // namespace vox::ui
