//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef input_single_scalar_h
#define input_single_scalar_h

#include "ui/widgets/data_widget.h"
#include "event.h"

namespace vox {
namespace ui {
/**
 * Input widget of generic type
 */
template<typename T>
class InputSingleScalar : public DataWidget<T> {
    static_assert(std::is_scalar<T>::value, "Invalid InputSingleScalar T (Scalar expected)");
    
public:
    /**
     * Constructor
     * @param p_dataType p_dataType
     * @param p_defaultValue p_defaultValue
     * @param p_step p_step
     * @param p_fastStep p_fastStep
     * @param p_label p_label
     * @param p_format p_format
     * @param p_selectAllOnClick p_selectAllOnClick
     */
    InputSingleScalar(ImGuiDataType p_dataType,
                      T p_defaultValue,
                      T p_step,
                      T p_fastStep,
                      const std::string &p_label,
                      const std::string &p_format,
                      bool p_selectAllOnClick) :
    DataWidget<T>(value), _dataType(p_dataType), value(p_defaultValue), step(p_step),
    fastStep(p_fastStep), label(p_label), format(p_format), selectAllOnClick(p_selectAllOnClick) {
    }
    
protected:
    void _draw_Impl() override {
        T previousValue = value;
        
        ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
        if (selectAllOnClick)
            flags |= ImGuiInputTextFlags_AutoSelectAll;
        
        bool enterPressed = ImGui::InputScalar((label + DataWidget<T>::_widgetID).c_str(), _dataType, &value,
                                               step != 0.0f ? &step : nullptr,
                                               fastStep != 0.0f ? &fastStep : nullptr,
                                               format.c_str(), flags);
        
        if (previousValue != value) {
            contentChangedEvent.invoke(value);
            DataWidget<T>::notifyChange();
        }
        
        if (enterPressed)
            enterPressedEvent.invoke(value);
    }
    
public:
    T value;
    T step;
    T fastStep;
    std::string label;
    std::string format;
    bool selectAllOnClick;
    Event<T> contentChangedEvent;
    Event<T> enterPressedEvent;
    
private:
    ImGuiDataType _dataType;
};

}
}
#endif /* input_single_scalar_h */
