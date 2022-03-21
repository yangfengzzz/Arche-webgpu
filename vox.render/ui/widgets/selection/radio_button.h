//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef radio_button_hpp
#define radio_button_hpp

#include "ui/widgets/data_widget.h"
#include "event.h"

namespace vox {
namespace ui {
/**
 * Widget that represent a button that is selectable. Only one radio button can be selected
 * in the same RadioButtonLinker
 */
class RadioButton : public DataWidget<bool> {
    friend class RadioButtonLinker;
    
public:
    /**
     * Constructor
     * @param p_selected p_selected
     * @param p_label p_label
     */
    RadioButton(bool p_selected = false, const std::string &p_label = "");
    
    /**
     * Make the radio button selected
     */
    void select();
    
    /**
     * Returns true if the radio button is selected
     */
    bool isSelected() const;
    
    /**
     * Returns the radio button ID
     */
    bool radioID() const;
    
protected:
    void _draw_Impl() override;
    
public:
    std::string label;
    Event<int> clickedEvent;
    
private:
    bool _selected = false;
    int _radioID = 0;
};


}
}
#endif /* radio_button_hpp */
