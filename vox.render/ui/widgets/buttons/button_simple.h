//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef button_simple_hpp
#define button_simple_hpp

#include "button.h"
#include "vector2.h"
#include "color.h"

namespace vox {
namespace ui {
/**
 * Simple button widget
 */
class ButtonSimple : public Button {
public:
    /**
     * Constructor
     * @param p_label p_label
     * @param p_size p_size
     * @param p_disabled p_disabled
     */
    ButtonSimple(const std::string &p_label = "",
                 const Vector2F &p_size = Vector2F(0.f, 0.f),
                 bool p_disabled = false);
    
protected:
    void _draw_Impl() override;
    
public:
    std::string label;
    Vector2F size;
    bool disabled = false;
    
    Color idleBackgroundColor;
    Color hoveredBackgroundColor;
    Color clickedBackgroundColor;
    
    Color textColor;
};


}
}
#endif /* button_simple_hpp */
