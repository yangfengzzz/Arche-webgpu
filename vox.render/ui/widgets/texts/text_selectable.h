//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef text_selectable_hpp
#define text_selectable_hpp

#include "text.h"

namespace vox {
namespace ui {
/**
 * Simple widget to display a selectable text on a panel
 */
class TextSelectable : public Text {
public:
    /**
     * Constructor
     * @param p_content p_content
     * @param p_selected p_selected
     * @param p_disabled p_disabled
     */
    TextSelectable(const std::string &p_content = "",
                   bool p_selected = false, bool p_disabled = false);
    
protected:
    void _draw_Impl() override;
    
public:
    bool selected;
    bool disabled;
    
    Event<bool> clickedEvent;
    Event<> selectedEvent;
    Event<> unselectedEvent;
};


}
}
#endif /* text_selectable_hpp */
