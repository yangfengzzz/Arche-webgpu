//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef button_small_hpp
#define button_small_hpp

#include "button.h"
#include "vector2.h"
#include "color.h"

namespace vox {
namespace ui {
/**
 * Small button widget
 */
class ButtonSmall : public Button {
public:
    /**
     * Constructor
     */
    ButtonSmall(const std::string &p_label = "");
    
protected:
    void _draw_Impl() override;
    
public:
    std::string label;
    
    Color idleBackgroundColor;
    Color hoveredBackgroundColor;
    Color clickedBackgroundColor;
    
    Color textColor;
};

}
}
#endif /* button_small_hpp */
