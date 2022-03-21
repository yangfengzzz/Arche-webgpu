//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef slider_single_scalar_h
#define slider_single_scalar_h

#include "vector2.h"
#include "event.h"

#include "ui/widgets/data_widget.h"
#include "ui/widgets/converter.h"

namespace vox {
namespace ui {
/**
 * Defines the slider orientation
 */
enum class SliderOrientation {
    HORIZONTAL,
    VERTICAL
};

/**
 * Slider widget of generic type
 */
template<typename T>
class SliderSingleScalar : public DataWidget<T> {
    static_assert(std::is_scalar<T>::value, "Invalid SliderSingleScalar T (Scalar expected)");
    
public:
    /**
     * Constructor
     * @param p_dataType p_dataType
     * @param p_min p_min
     * @param p_max p_max
     * @param p_value p_value
     * @param p_orientation p_orientation
     * @param p_label p_label
     * @param p_format p_format
     */
    SliderSingleScalar(ImGuiDataType p_dataType,
                       T p_min,
                       T p_max,
                       T p_value,
                       SliderOrientation p_orientation,
                       const std::string &p_label,
                       const std::string &p_format) :
    DataWidget<T>(value), _dataType(p_dataType), min(p_min), max(p_max),
    value(p_value), orientation(p_orientation), label(p_label), format(p_format) {
    }
    
protected:
    void _draw_Impl() override {
        if (max < min)
            max = min;
        
        if (value < min)
            value = min;
        else if (value > max)
            value = max;
        
        bool valueChanged = false;
        
        switch (orientation) {
            case SliderOrientation::HORIZONTAL:
                valueChanged = ImGui::SliderScalar((label + DataWidget<T>::_widgetID).c_str(),
                                                   _dataType, &value, &min, &max, format.c_str());
                break;
            case SliderOrientation::VERTICAL:
                valueChanged = ImGui::VSliderScalar((label + DataWidget<T>::_widgetID).c_str(),
                                                    Converter::ToImVec2(verticalModeSize),
                                                    _dataType, &value, &min, &max, format.c_str());
                break;
        }
        
        if (valueChanged) {
            valueChangedEvent.invoke(value);
            DataWidget<T>::notifyChange();
        }
    }
    
public:
    T min;
    T max;
    T value;
    SliderOrientation orientation;
    Vector2F verticalModeSize; /* Only applied with SliderOrientation::VERTICAL */
    std::string label;
    std::string format;
    Event<T> valueChangedEvent;
    
private:
    ImGuiDataType _dataType;
};


}
}

#endif /* slider_single_scalar_h */
