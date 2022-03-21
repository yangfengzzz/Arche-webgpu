//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "check_box.h"


namespace vox {
namespace ui {
CheckBox::CheckBox(bool p_value, const std::string &p_label) :
DataWidget<bool>(value), value(p_value), label(p_label) {
}

void CheckBox::_draw_Impl() {
    bool previousValue = value;
    
    ImGui::Checkbox((label + _widgetID).c_str(), &value);
    
    if (value != previousValue) {
        valueChangedEvent.invoke(value);
        notifyChange();
    }
}


}
}
