//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef text_disabled_hpp
#define text_disabled_hpp

#include "text.h"

namespace vox {
namespace ui {
/**
 * Widget to display a disabled text on a panel
 */
class TextDisabled : public Text {
public:
    /**
     * Constructor
     * @param p_content p_content
     */
    TextDisabled(const std::string &p_content = "");
    
protected:
    void _draw_Impl() override;
};


}
}
#endif /* text_disabled_hpp */
