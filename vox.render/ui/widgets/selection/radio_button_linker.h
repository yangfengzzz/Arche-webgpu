//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef radio_button_linker_hpp
#define radio_button_linker_hpp

#include "radio_button.h"

namespace vox {
namespace ui {
/**
 * Handle the link of radio buttons. Necessary to enable the correct behaviour
 * of radio buttons
 */
class RadioButtonLinker : public DataWidget<int> {
public:
    /**
     * Constructor
     */
    RadioButtonLinker();
    
    /**
     * Link the given radio button
     */
    void link(RadioButton &p_radioButton);
    
    /**
     * Unlink the given radio button
     */
    void unlink(RadioButton &p_radioButton);
    
    /**
     * Returns the id of the selected radio button
     */
    int selected() const;
    
protected:
    void _draw_Impl() override;
    
private:
    void onRadioButtonClicked(int p_radioID);
    
public:
    Event<int> valueChangedEvent;
    
private:
    int _availableRadioID = 0;
    int _selected = -1;
    std::vector<std::pair<ListenerID, std::reference_wrapper<RadioButton>>> _radioButtons;
};

}
}
#endif /* radio_button_linker_hpp */
