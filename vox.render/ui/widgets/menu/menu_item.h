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
 * Widget that can be added to a menu list. It is clickable and can be checkable
 */
class MenuItem : public DataWidget<bool> {
public:
    explicit MenuItem(std::string name, std::string shortcut = "", bool checkable = false, bool checked = false);

protected:
    void DrawImpl() override;

public:
    std::string name_;
    std::string shortcut_;
    bool checkable_;
    bool checked_;
    Event<> clicked_event_;
    Event<bool> value_changed_event_;

private:
    bool selected_{};
};

}  // namespace vox::ui
