//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/menu/menu_list.h"

#include <utility>

namespace vox::ui {
MenuList::MenuList(std::string name, bool locked) : name_(std::move(name)), locked_(locked) {}

void MenuList::DrawImpl() {
    if (ImGui::BeginMenu(name_.c_str(), !locked_)) {
        if (!opened_) {
            clicked_event_.Invoke();
            opened_ = true;
        }

        DrawWidgets();
        ImGui::EndMenu();
    } else {
        opened_ = false;
    }
}

}  // namespace vox::ui
