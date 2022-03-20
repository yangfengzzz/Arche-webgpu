//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "input_text.h"

namespace vox {
namespace ui {
InputText::InputText(const std::string &p_content, const std::string &p_label) :
DataWidget<std::string>(content), content(p_content), label(p_label) {
}

void InputText::_draw_Impl() {
    std::string previousContent = content;
    
    content.resize(256, '\0');
    bool enterPressed = ImGui::InputText((label + _widgetID).c_str(), &content[0], 256,
                                         ImGuiInputTextFlags_EnterReturnsTrue | (selectAllOnClick ? ImGuiInputTextFlags_AutoSelectAll : 0));
    content = content.c_str();
    
    if (content != previousContent) {
        contentChangedEvent.invoke(content);
        notifyChange();
    }
    
    if (enterPressed)
        enterPressedEvent.invoke(content);
}

}
}
