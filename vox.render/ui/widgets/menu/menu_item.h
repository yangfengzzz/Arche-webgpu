//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef menu_item_hpp
#define menu_item_hpp

#include "ui/widgets/data_widget.h"
#include "event.h"

namespace vox {
namespace ui {
/**
 * Widget that can be added to a menu list. It is clickable and can be checkable
 */
class MenuItem : public DataWidget<bool> {
public:
    /**
     * Constructor
     * @param p_name p_name
     * @param p_shortcut p_shortcut
     * @param p_checkable p_checkable
     * @param p_checked p_checked
     */
    MenuItem(const std::string &p_name, const std::string &p_shortcut = "",
             bool p_checkable = false, bool p_checked = false);
    
protected:
    void _draw_Impl() override;
    
public:
    std::string name;
    std::string shortcut;
    bool checkable;
    bool checked;
    Event<> clickedEvent;
    Event<bool> valueChangedEvent;
    
private:
    bool m_selected;
};


}
}
#endif /* menu_item_hpp */
