//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "button_colored.h"
#include "ui/widgets/converter.h"

namespace vox {
namespace ui {
ButtonColored::ButtonColored(const std::string &p_label, const Color &p_color,
                             const Vector2F &p_size, bool p_enableAlpha) :
label(p_label), color(p_color), size(p_size), enableAlpha(p_enableAlpha) {
}

void ButtonColored::_draw_Impl() {
    ImVec4 imColor = Converter::ToImVec4(color);
    
    if (ImGui::ColorButton((label + _widgetID).c_str(), imColor,
                           !enableAlpha ? ImGuiColorEditFlags_NoAlpha : 0,
                           Converter::ToImVec2(size)))
        clickedEvent.invoke();
    
    color = Converter::ToColor(imColor);
}


}
}
