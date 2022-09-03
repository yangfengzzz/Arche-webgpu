//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/panels/panel_menu_bar.h"

namespace vox::ui {
void PanelMenuBar::DrawImpl() {
    if (!widgets_.empty() && ImGui::BeginMainMenuBar()) {
        DrawWidgets();
        ImGui::EndMainMenuBar();
    }
}

}  // namespace vox::ui
