//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef button_colored_hpp
#define button_colored_hpp

#include "button.h"
#include "vector2.h"
#include "color.h"

namespace vox {
namespace ui {
/**
 * Button widget of a single color (Color palette element)
 */
class ButtonColored : public Button {
public:
    /**
     * Constructor
     * @param p_label p_label
     * @param p_color p_color
     * @param p_size p_size
     * @param p_enableAlpha p_enableAlpha
     */
    ButtonColored(const std::string &p_label = "", const Color &p_color = {},
                  const Vector2F &p_size = Vector2F(0.f, 0.f), bool p_enableAlpha = true);
    
protected:
    void _draw_Impl() override;
    
public:
    std::string label;
    Color color;
    Vector2F size;
    bool enableAlpha;
};


}
}
#endif /* button_colored_hpp */
