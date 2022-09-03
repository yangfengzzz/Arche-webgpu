//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/event.h"
#include "vox.render/ui/widgets/layout/group.h"

namespace vox::ui {
/**
 * Widget that behave like a group with a menu display
 */
class MenuList : public Group {
public:
    explicit MenuList(std::string name, bool locked = false);

protected:
    void DrawImpl() override;

public:
    std::string name_;
    bool locked_;
    Event<> clicked_event_;

private:
    bool opened_{};
};

}  // namespace vox::ui
