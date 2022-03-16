//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef main_menu_hpp
#define main_menu_hpp

#include <stdio.h>

namespace vox {
namespace editor {
class GUIEntry;
class MainMenu {
public:
    MainMenu(GUIEntry *entry);
    
    void showMainMenu();
    
    void showFileMenu();
    
    void showOptionsMenu();
    
    void showWindowsMenu();
    
    void showHelpMenu();
    
private:
    GUIEntry *_entry{nullptr};
};

}
}
#endif /* main_menu_hpp */
