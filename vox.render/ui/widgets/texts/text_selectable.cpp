//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "text_selectable.h"

namespace vox {
namespace ui {
TextSelectable::TextSelectable(const std::string &p_content, bool p_selected, bool p_disabled) :
Text(p_content), selected(p_selected), disabled(p_disabled) {
}

void TextSelectable::_draw_Impl() {
    if (ImGui::Selectable((content + _widgetID).c_str(), &selected,
                          disabled ? ImGuiSelectableFlags_Disabled : ImGuiSelectableFlags_None)) {
        clickedEvent.invoke(selected);
        
        if (selected)
            selectedEvent.invoke();
        else
            unselectedEvent.invoke();
    }
}


}
}
