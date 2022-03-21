//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef text_hpp
#define text_hpp

#include "ui/widgets/data_widget.h"

namespace vox {
namespace ui {
/**
 * Simple widget to display text on a panel
 */
class Text : public DataWidget<std::string> {
public:
    /**
     * Constructor
     * @param p_content p_content
     */
    Text(const std::string &p_content = "");
    
protected:
    void _draw_Impl() override;
    
public:
    std::string content;
};

}
}
#endif /* text_hpp */
