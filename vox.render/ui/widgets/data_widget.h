//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef data_widget_h
#define data_widget_h

#include "button.h"

namespace vox {
namespace ui {
/**
 * DataWidget are widgets that contains a value. It is very usefull in combination with
 * DataDispatcher plugin
 */
template<typename T>
class DataWidget : public Widget {
public:
    /**
     * Create a DataWidget with the data specification
     * @param p_dataHolder p_dataHolder
     */
    DataWidget(T &p_dataHolder) : _data(p_dataHolder) {
    };
    
    /**
     * Draw the widget
     */
    void draw() override;
    
    /**
     * Notify that the widget data has changed to allow the data dispatcher to execute its behaviour
     */
    void notifyChange();
    
private:
    T &_data;
};

template<typename T>
inline void DataWidget<T>::draw() {
    if (enabled) {
        TRY_GATHER(T, _data);
        Widget::draw();
        TRY_PROVIDE(T, _data);
    }
}

template<typename T>
inline void DataWidget<T>::notifyChange() {
    TRY_NOTIFY_CHANGE(T);
}


}
}
#endif /* data_widget_h */
