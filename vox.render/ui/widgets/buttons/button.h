//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef button_h
#define button_h

#include "ui/widgets/widget.h"
#include "event.h"

namespace vox {
namespace ui {
/**
 * Base class for any button widget
 */
class Button : public Widget {
public:
    Event<> ClickedEvent;
};


}
}
#endif /* button_h */
