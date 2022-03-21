//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef check_box_hpp
#define check_box_hpp

#include "ui/widgets/data_widget.h"
#include "event.h"

namespace vox {
namespace ui {
/**
 * Checkbox widget that can be checked or not
 */
class CheckBox : public DataWidget<bool> {
public:
    /**
     * Constructor
     * @param p_value p_value
     * @param p_label p_label
     */
    CheckBox(bool p_value = false, const std::string &p_label = "");
    
protected:
    void _draw_Impl() override;
    
public:
    bool value;
    std::string label;
    Event<bool> valueChangedEvent;
};


}
}
#endif /* check_box_hpp */
