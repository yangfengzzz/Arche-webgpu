//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef input_text_hpp
#define input_text_hpp

#include "ui/widgets/data_widget.h"
#include "event.h"

namespace vox {
namespace ui {
/**
 * Input widget of type string
 */
class InputText : public DataWidget<std::string> {
public:
    /**
     * Constructor
     * @param p_content p_content
     * @param p_label p_label
     */
    InputText(const std::string &p_content = "", const std::string &p_label = "");
    
protected:
    void _draw_Impl() override;
    
public:
    std::string content;
    std::string label;
    bool selectAllOnClick = false;
    Event<std::string> contentChangedEvent;
    Event<std::string> enterPressedEvent;
};

}
}
#endif /* input_text_hpp */
