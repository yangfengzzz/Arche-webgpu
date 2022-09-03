//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/layout/group_collapsable.h"

#include <utility>

namespace vox::ui {
GroupCollapsable::GroupCollapsable(std::string name) : name_(std::move(name)) {}

void GroupCollapsable::DrawImpl() {
    bool previously_opened = opened_;

    if (ImGui::CollapsingHeader(name_.c_str(), closable_ ? &opened_ : nullptr)) Group::DrawImpl();

    if (opened_ != previously_opened) {
        if (opened_)
            open_event_.Invoke();
        else
            close_event_.Invoke();
    }
}

}  // namespace vox::ui
