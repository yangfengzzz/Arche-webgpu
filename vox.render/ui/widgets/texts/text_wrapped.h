//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef text_wrapped_hpp
#define text_wrapped_hpp

#include "text.h"

namespace vox {
namespace ui {
/**
 * Simple widget to display a long text with word-wrap on a panel
 */
class TextWrapped : public Text {
public:
    /**
     * Constructor
     * @param p_content p_content
     */
    TextWrapped(const std::string &p_content = "");
    
protected:
    void _draw_Impl() override;
};


}
}
#endif /* text_wrapped_hpp */
