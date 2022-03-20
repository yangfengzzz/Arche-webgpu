//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "group_collapsable.h"

namespace vox {
namespace ui {
GroupCollapsable::GroupCollapsable(const std::string &p_name) :
name(p_name) {
}

void GroupCollapsable::_draw_Impl() {
    bool previouslyOpened = opened;
    
    if (ImGui::CollapsingHeader(name.c_str(), closable ? &opened : nullptr))
        Group::_draw_Impl();
    
    if (opened != previouslyOpened) {
        if (opened)
            openEvent.invoke();
        else
            closeEvent.invoke();
    }
}

}
}
