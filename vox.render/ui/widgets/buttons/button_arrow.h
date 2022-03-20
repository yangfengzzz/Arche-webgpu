//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef button_arrow_hpp
#define button_arrow_hpp

#include "button.h"

namespace vox {
namespace ui {
/**
 * Button widget with an arrow image
 */
class ButtonArrow : public Button {
public:
    /**
     * Create the button
     */
    ButtonArrow(ImGuiDir p_direction = ImGuiDir_None);
    
protected:
    void _draw_Impl() override;
    
public:
    ImGuiDir direction;
};


}
}
#endif /* button_arrow_hpp */
