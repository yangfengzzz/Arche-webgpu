//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef text_labelled_hpp
#define text_labelled_hpp

#include "text.h"

namespace vox {
namespace ui {
/**
 * Simple widget to display a labelled text on a panel
 */
class TextLabelled : public Text {
public:
    /**
     * Constructor
     * @param p_content p_content
     * @param p_label p_label
     */
    TextLabelled(const std::string &p_content = "", const std::string &p_label = "");
    
protected:
    void _draw_Impl() override;
    
public:
    std::string label;
};

}
}
#endif /* text_labelled_hpp */
