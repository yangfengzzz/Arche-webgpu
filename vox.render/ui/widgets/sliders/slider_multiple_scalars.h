//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef slider_multiple_scalars_h
#define slider_multiple_scalars_h

#include "event.h"
#include "ui/widgets/data_widget.h"
#include <array>

namespace vox {
namespace ui {
/**
 * Slider widget of multiple generic type
 */
template<typename T, size_t _Size>
class SliderMultipleScalars : public DataWidget<std::array<T, _Size>> {
    static_assert(_Size > 1, "Invalid SliderMultipleScalars _Size (2 or more requiered)");
    
public:
    /**
     * Constructor
     * @param p_dataType
     * @param p_min
     * @param p_max
     * @param p_value
     * @param p_label
     * @param p_format
     */
    SliderMultipleScalars
    (
     ImGuiDataType_ p_dataType,
     T p_min,
     T p_max,
     T p_value,
     const std::string &p_label,
     const std::string &p_format
     )
    : DataWidget<std::array<T, _Size>>(values),
    m_dataType(p_dataType),
    min(p_min),
    max(p_max),
    label(p_label),
    format(p_format) {
        values.fill(p_value);
    }
    
protected:
    void _draw_Impl() override {
        if (max < min)
            max = min;
        
        for (size_t i = 0; i < _Size; ++i) {
            if (values[i] < min)
                values[i] = min;
            else if (values[i] > max)
                values[i] = max;
        }
        
        bool valueChanged = ImGui::SliderScalarN((label + this->m_widgetID).c_str(), m_dataType, values.data(), _Size, &min, &max, format.c_str());
        
        if (valueChanged) {
            valueChangedEvent.invoke(values);
            notifyChange();
        }
    }
    
public:
    T min;
    T max;
    std::array<T, _Size> values;
    std::string label;
    std::string format;
    Event<std::array<T, _Size> &> valueChangedEvent;
    
protected:
    ImGuiDataType_ _dataType;
};


}
}
#endif /* slider_multiple_scalars_h */
