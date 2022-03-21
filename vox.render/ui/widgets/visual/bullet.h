//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef bullet_hpp
#define bullet_hpp

#include "ui/widgets/widget.h"

namespace vox {
namespace ui {
/**
 * Simple widget that display a bullet point
 */
class Bullet : public Widget {
protected:
    void _draw_Impl() override;
};

}
}
#endif /* bullet_hpp */
