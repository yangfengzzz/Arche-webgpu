//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef input_multiple_scalars_h
#define input_multiple_scalars_h

#include "ui/widgets/data_widget.h"
#include "event.h"

namespace vox {
namespace ui {
/**
 * Input widget of multiple generic type
 */
template<typename T, size_t _Size>
class InputMultipleScalars : public DataWidget<std::array<T, _Size>> {
    static_assert(_Size > 1, "Invalid InputMultipleScalars _Size (2 or more requiered)");
    static_assert(std::is_scalar<T>::value, "Invalid InputMultipleScalars T (Scalar expected)");
    
public:
    /**
     * Constructor
     * @param p_dataType
     * @param p_defaultValue
     * @param p_step
     * @param p_fastStep
     * @param p_label
     * @param p_format
     * @param p_selectAllOnClick
     */
    InputMultipleScalars(ImGuiDataType p_dataType,
                         T p_defaultValue,
                         T p_step,
                         T p_fastStep,
                         const std::string &p_label,
                         const std::string &p_format,
                         bool p_selectAllOnClick) :
    DataWidget<std::array<T, _Size>>(values),
    m_dataType(p_dataType),
    step(p_step),
    fastStep(p_fastStep),
    label(p_label),
    format(p_format),
    selectAllOnClick(p_selectAllOnClick) {
        values.fill(p_defaultValue);
    }
    
protected:
    void _draw_Impl() override {
        std::array<T, _Size> previousValue = values;
        
        ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
        if (selectAllOnClick)
            flags |= ImGuiInputTextFlags_AutoSelectAll;
        
        bool enterPressed = ImGui::InputScalarN((label + _widgetID).c_str(), m_dataType, values.data(),
                                                _Size, step != 0.0f ? &step : nullptr,
                                                fastStep != 0.0f ? &fastStep : nullptr, format.c_str(), flags);
        
        bool hasChanged = false;
        
        for (size_t i = 0; i < _Size; ++i)
            if (previousValue[i] != values[i])
                hasChanged = true;
        
        if (hasChanged) {
            ContentChangedEvent.Invoke(values);
            this->NotifyChange();
        }
        
        if (enterPressed)
            EnterPressedEvent.Invoke(values);
    }
    
public:
    std::array<T, _Size> values;
    T step;
    T fastStep;
    std::string label;
    std::string format;
    bool selectAllOnClick;
    Event<std::array<T, _Size> &> contentChangedEvent;
    Event<std::array<T, _Size> &> enterPressedEvent;
    
private:
    ImGuiDataType _dataType;
};


}
}
#endif /* input_multiple_scalars_h */
