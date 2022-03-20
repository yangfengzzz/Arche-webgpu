//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef dummy_hpp
#define dummy_hpp

#include "vector2.h"

#include "ui/widgets/widget.h"

namespace vox {
namespace ui {
/**
 * Dummy widget that takes the given size as space in the panel
 */
class Dummy : public Widget {
public:
    /**
     * Constructor
     * @param p_size p_size
     */
    Dummy(const Vector2F &p_size = {0.0f, 0.0f});
    
protected:
    void _draw_Impl() override;
    
public:
    Vector2F size;
};


}
}
#endif /* dummy_hpp */
