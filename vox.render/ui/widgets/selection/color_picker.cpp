//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "color_picker.h"

namespace vox {
namespace ui {
ColorPicker::ColorPicker(bool p_enableAlpha, const Color &p_defaultColor) :
DataWidget<Color>(color), enableAlpha(p_enableAlpha), color(p_defaultColor) {
}

void ColorPicker::_draw_Impl() {
    int flags = !enableAlpha ? ImGuiColorEditFlags_NoAlpha : 0;
    bool valueChanged = false;
    
    if (enableAlpha)
        valueChanged = ImGui::ColorPicker4(_widgetID.c_str(), &color.r, flags);
    else
        valueChanged = ImGui::ColorPicker3(_widgetID.c_str(), &color.r, flags);
    
    if (valueChanged) {
        colorChangedEvent.invoke(color);
        notifyChange();
    }
}

}
}
