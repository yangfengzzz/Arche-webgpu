//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef drag_single_scalar_h
#define drag_single_scalar_h

#include "ui/widgets/data_widget.h"
#include "event.h"

namespace vox {
namespace ui {
/**
 * Drag widget of generic type
 */
template<typename T>
class DragSingleScalar : public DataWidget<T> {
    static_assert(std::is_scalar<T>::value, "Invalid DragSingleScalar T (Scalar expected)");
    
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
    DragSingleScalar(ImGuiDataType p_dataType,
                     T p_min,
                     T p_max,
                     T p_value,
                     float p_speed,
                     const std::string &p_label,
                     const std::string &p_format)
    : DataWidget<T>(value), _dataType(p_dataType), min(p_min), max(p_max),
    value(p_value), speed(p_speed), label(p_label), format(p_format) {
    }
    
protected:
    void _draw_Impl() override {
        if (max < min)
            max = min;
        
        if (value < min)
            value = min;
        else if (value > max)
            value = max;
        
        if (ImGui::DragScalar((label + DataWidget<T>::_widgetID).c_str(),
                              _dataType, &value, speed, &min, &max, format.c_str())) {
            valueChangedEvent.invoke(value);
            DataWidget<T>::notifyChange();
        }
    }
    
public:
    T min;
    T max;
    T value;
    float speed;
    std::string label;
    std::string format;
    Event<T> valueChangedEvent;
    
private:
    ImGuiDataType _dataType;
};


}
}
#endif /* drag_single_scalar_h */
