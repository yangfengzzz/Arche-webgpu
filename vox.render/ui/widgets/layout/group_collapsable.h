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
 * Widget that can contains other widgets and is collapsable
 */
class GroupCollapsable : public Group {
public:
    explicit GroupCollapsable(std::string name = "");

protected:
    void DrawImpl() override;

public:
    std::string name_;
    bool closable_ = false;
    bool opened_ = true;
    Event<> close_event_;
    Event<> open_event_;
};

}  // namespace vox::ui
