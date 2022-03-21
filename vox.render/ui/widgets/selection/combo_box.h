//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef combo_box_hpp
#define combo_box_hpp

#include "ui/widgets/data_widget.h"
#include "event.h"
#include <map>

namespace vox {
namespace ui {
/**
 * Widget that can display a list of values that the user can select
 */
class ComboBox : public DataWidget<int> {
public:
    /**
     * Constructor
     * @param p_currentChoice p_currentChoice
     */
    ComboBox(int p_currentChoice = 0);
    
protected:
    void _draw_Impl() override;
    
public:
    std::map<int, std::string> choices;
    int currentChoice;
    
public:
    Event<int> valueChangedEvent;
};


}
}
#endif /* combo_box_hpp */
