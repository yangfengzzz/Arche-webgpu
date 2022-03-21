//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "text_clickable.h"


namespace vox {
namespace ui {
TextClickable::TextClickable(const std::string &p_content) :
Text(p_content) {
}

void TextClickable::_draw_Impl() {
    bool useless = false;
    
    if (ImGui::Selectable((content + _widgetID).c_str(), &useless,
                          ImGuiSelectableFlags_AllowDoubleClick)) {
        if (ImGui::IsMouseDoubleClicked(0)) {
            doubleClickedEvent.invoke();
        } else {
            clickedEvent.invoke();
        }
    }
}


}
}
