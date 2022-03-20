//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef group_hpp
#define group_hpp

#include "ui/widgets/widget_container.h"

namespace vox {
namespace ui {
/**
 * Widget that can contains other widgets
 */
class Group : public Widget, public WidgetContainer {
protected:
    virtual void _draw_Impl() override;
};

}
}
#endif /* group_hpp */
