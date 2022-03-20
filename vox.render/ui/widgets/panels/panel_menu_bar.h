//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef panel_menu_bar_hpp
#define panel_menu_bar_hpp

#include "ui/widgets/panel.h"

namespace vox {
namespace ui {
/**
 * A simple panel that will be displayed on the top side of the canvas
 */
class PanelMenuBar : public Panel {
protected:
    void _draw_Impl() override;
};


}
}
#endif /* panel_menu_bar_hpp */
