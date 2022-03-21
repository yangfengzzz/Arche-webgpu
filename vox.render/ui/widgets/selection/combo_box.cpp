//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "combo_box.h"

namespace vox {
namespace ui {
ComboBox::ComboBox(int p_currentChoice) :
DataWidget<int>(currentChoice), currentChoice(p_currentChoice) {
    
}

void ComboBox::_draw_Impl() {
    if (choices.find(currentChoice) == choices.end())
        currentChoice = choices.begin()->first;
    
    if (ImGui::BeginCombo(_widgetID.c_str(), choices[currentChoice].c_str())) {
        for (const auto&[key, value]: choices) {
            bool selected = key == currentChoice;
            
            if (ImGui::Selectable(value.c_str(), selected)) {
                if (!selected) {
                    ImGui::SetItemDefaultFocus();
                    currentChoice = key;
                    valueChangedEvent.invoke(currentChoice);
                    notifyChange();
                }
            }
        }
        
        ImGui::EndCombo();
    }
}


}
}
