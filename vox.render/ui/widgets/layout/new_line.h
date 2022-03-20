//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef new_line_hpp
#define new_line_hpp

#include "ui/widgets/widget.h"

namespace vox {
namespace ui {
/**
 * Widget that adds an empty line to the panel
 */
class NewLine : public Widget {
protected:
    void _draw_Impl() override;
};


}
}
#endif /* new_line_hpp */
