//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/menu/menu_item.h"

#include <utility>

namespace vox::ui {
MenuItem::MenuItem(std::string name, std::string shortcut, bool checkable, bool checked)
    : DataWidget(selected_),
      name_(std::move(name)),
      shortcut_(std::move(shortcut)),
      checkable_(checkable),
      checked_(checked) {}

void MenuItem::DrawImpl() {
    bool previous_value = checked_;

    if (ImGui::MenuItem((name_ + widget_id_).c_str(), shortcut_.c_str(), checkable_ ? &checked_ : nullptr, enabled_))
        clicked_event_.Invoke();

    if (checked_ != previous_value) {
        value_changed_event_.Invoke(checked_);
        NotifyChange();
    }
}

}  // namespace vox::ui
