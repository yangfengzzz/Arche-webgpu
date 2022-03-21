//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef text_colored_hpp
#define text_colored_hpp

#include "text.h"
#include "color.h"

namespace vox {
namespace ui {
/**
 * Widget to display text on a panel that can be colored
 */
class TextColored : public Text {
public:
    /**
     * Constructor
     * @param p_content p_content
     * @param p_color p_color
     */
    TextColored(const std::string &p_content = "",
                const Color &p_color = Color(1.0f, 1.0f, 1.0f, 1.0f));
    
public:
    Color color;
    
protected:
    void _draw_Impl() override;
};


}
}
#endif /* text_colored_hpp */
