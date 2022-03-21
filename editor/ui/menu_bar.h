//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef menu_bar_hpp
#define menu_bar_hpp

#include "ui/widgets/panels/panel_menu_bar.h"
#include "ui/widgets/panel_transformables/panel_window.h"
#include "ui/widgets/menu/menu_item.h"
#include "ui/widgets/menu/menu_list.h"

namespace vox {
using namespace ui;

namespace editor {
namespace ui {
class MenuBar : public PanelMenuBar {
    using PanelMap = std::unordered_map<std::string, std::pair<std::reference_wrapper<PanelWindow>, std::reference_wrapper<MenuItem>>>;
    
public:
    /**
     * Constructor
     */
    MenuBar();
    
    /**
     * Check inputs for menubar shortcuts
     * @param p_deltaTime p_deltaTime
     */
    void handleShortcuts(float p_deltaTime);
    
    /**
     * Register a panel to the menu bar window menu
     */
    void registerPanel(const std::string &p_name, PanelWindow &p_panel);
    
private:
    void createFileMenu();
    
    void createBuildMenu();
    
    void createWindowMenu();
    
    void createActorsMenu();
    
    void createResourcesMenu();
    
    void createSettingsMenu();
    
    void createLayoutMenu();
    
    void createHelpMenu();
    
    void updateToggleableItems();
    
    void openEveryWindows(bool p_state);
    
private:
    PanelMap _panels;
    
    MenuList *_windowMenu{nullptr};
};

}
}
}
#endif /* menu_bar_hpp */
