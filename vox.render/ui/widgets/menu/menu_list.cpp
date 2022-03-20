//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "menu_list.h"

namespace vox {
namespace ui {
MenuList::MenuList(const std::string &p_name, bool p_locked) :
name(p_name), locked(p_locked) {
}

void MenuList::_draw_Impl() {
    if (ImGui::BeginMenu(name.c_str(), !locked)) {
        if (!m_opened) {
            clickedEvent.invoke();
            m_opened = true;
        }
        
        drawWidgets();
        ImGui::EndMenu();
    } else {
        m_opened = false;
    }
}

}
}
