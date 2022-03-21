//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "radio_button.h"

namespace vox {
namespace ui {
RadioButton::RadioButton(bool p_selected, const std::string &p_label) :
DataWidget<bool>(_selected), label(p_label) {
    if (p_selected)
        select();
}

void RadioButton::select() {
    _selected = true;
    clickedEvent.invoke(_radioID);
}

bool RadioButton::isSelected() const {
    return _selected;
}

bool RadioButton::radioID() const {
    return _radioID;
}

void RadioButton::_draw_Impl() {
    if (ImGui::RadioButton((label + _widgetID).c_str(), _selected)) {
        clickedEvent.invoke(_radioID);
        notifyChange();
    }
}


}
}
