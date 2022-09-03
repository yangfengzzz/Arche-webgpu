//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/event.h"
#include "vox.render/ui/widgets/widget.h"

namespace vox::ui {
/**
 * Base class for any button widget
 */
class Button : public Widget {
public:
    Event<> clicked_event_;
};

}  // namespace vox::ui
