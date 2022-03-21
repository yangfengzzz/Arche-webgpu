//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef drag_multiple_scalars_h
#define drag_multiple_scalars_h

#include "ui/widgets/data_widget.h"
#include "event.h"

namespace vox {
namespace ui {
/**
 * Drag widget of multiple generic type
 */
template<typename T, size_t _Size>
class DragMultipleScalars : public DataWidget<std::array<T, _Size>> {
    static_assert(_Size > 1, "Invalid DragMultipleScalars _Size (2 or more requiered)");
    
public:
    /**
     * Constructor
     * @param p_dataType p_dataType
     * @param p_min p_min
     * @param p_max p_max
     * @param p_value p_value
     * @param p_speed p_speed
     * @param p_label p_label
     * @param p_format p_format
     */
    DragMultipleScalars(ImGuiDataType_ p_dataType,
                        T p_min,
                        T p_max,
                        T p_value,
                        float p_speed,
                        const std::string &p_label,
                        const std::string &p_format) :
    DataWidget<std::array<T, _Size>>(values), _dataType(p_dataType),
    min(p_min), max(p_max), speed(p_speed), label(p_label), format(p_format) {
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
        
        if (ImGui::DragScalarN((label + DataWidget<std::array<T, _Size>>::_widgetID).c_str(),
                               _dataType, values.data(), _Size, speed, &min, &max, format.c_str())) {
            valueChangedEvent.invoke(values);
            DataWidget<std::array<T, _Size>>::notifyChange();
        }
    }
    
public:
    T min;
    T max;
    float speed;
    std::array<T, _Size> values;
    std::string label;
    std::string format;
    Event<std::array<T, _Size> &> valueChangedEvent;
    
protected:
    ImGuiDataType_ _dataType;
};


}
}
#endif /* drag_multiple_scalars_h */
