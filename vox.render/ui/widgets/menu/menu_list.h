//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef menu_list_hpp
#define menu_list_hpp

#include "ui/widgets/layout/group.h"
#include "event.h"

namespace vox {
namespace ui {
/**
 * Widget that behave like a group with a menu display
 */
class MenuList : public Group {
public:
    /**
     * Constructor
     * @param p_name p_name
     * @param p_locked p_locked
     */
    MenuList(const std::string &p_name, bool p_locked = false);
    
protected:
    void _draw_Impl() override;
    
public:
    std::string name;
    bool locked;
    Event<> clickedEvent;
    
private:
    bool m_opened;
};

}
}
#endif /* menu_list_hpp */
