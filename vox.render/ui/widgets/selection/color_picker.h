//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef color_picker_hpp
#define color_picker_hpp

#include "ui/widgets/data_widget.h"
#include "event.h"
#include "color.h"

namespace vox {
namespace ui {
/**
 * Widget that allow the selection of a color with a color picker
 */
class ColorPicker : public DataWidget<Color> {
public:
    /**
     * Constructor
     * @param p_enableAlpha p_enableAlpha
     * @param p_defaultColor p_defaultColor
     */
    ColorPicker(bool p_enableAlpha = false, const Color &p_defaultColor = {});
    
protected:
    void _draw_Impl() override;
    
public:
    bool enableAlpha;
    Color color;
    Event<Color &> colorChangedEvent;
};

}
}
#endif /* color_picker_hpp */
