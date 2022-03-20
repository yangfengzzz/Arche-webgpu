//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "menu_item.h"

namespace vox {
namespace ui {
MenuItem::MenuItem(const std::string &p_name, const std::string &p_shortcut,
                   bool p_checkable, bool p_checked) :
DataWidget(m_selected), name(p_name), shortcut(p_shortcut),
checkable(p_checkable), checked(p_checked) {
}

void MenuItem::_draw_Impl() {
    bool previousValue = checked;
    
    if (ImGui::MenuItem((name + _widgetID).c_str(), shortcut.c_str(),
                        checkable ? &checked : nullptr, enabled))
        clickedEvent.invoke();
    
    if (checked != previousValue) {
        valueChangedEvent.invoke(checked);
        notifyChange();
    }
}

}
}
