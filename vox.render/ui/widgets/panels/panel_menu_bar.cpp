//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "panel_menu_bar.h"


namespace vox {
namespace ui {
void PanelMenuBar::_draw_Impl() {
    if (!_widgets.empty() && ImGui::BeginMainMenuBar()) {
        drawWidgets();
        ImGui::EndMainMenuBar();
    }
}

}
}
