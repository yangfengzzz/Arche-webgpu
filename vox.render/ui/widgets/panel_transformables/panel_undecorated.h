//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef panel_undecorated_hpp
#define panel_undecorated_hpp

#include "ui/widgets/panel_transformable.h"

namespace vox {
namespace ui {
/**
 * A simple panel that is transformable and without decorations (No background)
 */
class PanelUndecorated : public PanelTransformable {
public:
    void _draw_Impl() override;
    
private:
    int collectFlags();
};


}
}
#endif /* panel_undecorated_hpp */
