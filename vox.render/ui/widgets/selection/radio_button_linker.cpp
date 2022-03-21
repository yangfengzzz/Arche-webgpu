//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "radio_button_linker.h"

namespace vox {
namespace ui {
RadioButtonLinker::RadioButtonLinker() : DataWidget<int>(_selected) {
}

void RadioButtonLinker::link(RadioButton &p_radioButton) {
    p_radioButton._radioID = _availableRadioID++;
    auto listenerID = p_radioButton.clickedEvent += std::bind(&RadioButtonLinker::onRadioButtonClicked, this, std::placeholders::_1);
    
    if (p_radioButton.isSelected() && _selected == -1)
        _selected = p_radioButton._radioID;
    
    _radioButtons.emplace_back(listenerID, std::ref(p_radioButton));
}

void RadioButtonLinker::unlink(RadioButton &p_radioButton) {
    auto it = std::find_if(_radioButtons.begin(), _radioButtons.end(),
                           [&p_radioButton](std::pair<ListenerID, std::reference_wrapper<RadioButton>> &p_pair) {
        return &p_pair.second.get() == &p_radioButton;
    });
    
    if (it != _radioButtons.end()) {
        it->second.get().clickedEvent.removeListener(it->first);
        _radioButtons.erase(it);
    }
}

int RadioButtonLinker::selected() const {
    return _selected;
}

void RadioButtonLinker::_draw_Impl() {
    // The RadioButtonLinker is special, it has nothing to display :)
}

void RadioButtonLinker::onRadioButtonClicked(int p_radioID) {
    if (p_radioID != _selected) {
        _selected = p_radioID;
        valueChangedEvent.invoke(_selected);
        notifyChange();
        
        for (const auto&[listener, radioButton]: _radioButtons)
            radioButton.get()._selected = radioButton.get()._radioID == _selected;
    }
}


}
}
