//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef contextual_menu_hpp
#define contextual_menu_hpp

#include "ui/plugins/plugin.h"
#include "ui/widgets/widget_container.h"
#include "ui/widgets/menu/menu_list.h"
#include "ui/widgets/menu/menu_item.h"

namespace vox {
namespace ui {
/**
 * A simple plugin that will show a contextual menu on right click
 * You can add widgets to a contextual menu
 */
class ContextualMenu : public Plugin, public WidgetContainer {
public:
    /**
     * Execute the plugin
     */
    void execute();
    
    /**
     * Force close the contextual menu
     */
    void close();
};


}
}
#endif /* contextual_menu_hpp */
