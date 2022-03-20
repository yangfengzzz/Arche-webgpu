//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef spacing_hpp
#define spacing_hpp

#include "ui/widgets/widget.h"

namespace vox {
namespace ui {
/**
 * Widget that adds a space to the panel line
 */
class Spacing : public Widget {
public:
    /**
     * Constructor
     * @param p_spaces p_spaces
     */
    Spacing(uint16_t p_spaces = 1);
    
protected:
    void _draw_Impl() override;
    
public:
    uint16_t spaces = 1;
};

}
}
#endif /* spacing_hpp */
