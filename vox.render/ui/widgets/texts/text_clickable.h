//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef text_clickable_hpp
#define text_clickable_hpp

#include "text.h"
#include "event.h"

namespace vox {
namespace ui {
/**
 * Widget to display text on a panel that is also clickable
 */
class TextClickable : public Text {
public:
    /**
     * Constructor
     * @param p_content p_content
     */
    TextClickable(const std::string &p_content = "");
    
protected:
    void _draw_Impl() override;
    
public:
    Event<> clickedEvent;
    Event<> doubleClickedEvent;
};


}
}
#endif /* text_clickable_hpp */
