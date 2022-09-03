//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/plugins/data_dispatcher.h"
#include "vox.render/ui/widgets/buttons/button.h"

namespace vox::ui {
/**
 * DataWidget are widgets that contains a value. It is very usefully in combination with
 * DataDispatcher plugin
 */
template <typename T>
class DataWidget : public Widget {
public:
    /**
     * Create a DataWidget with the data specification
     */
    explicit DataWidget(T &data_holder) : data_(data_holder){};

    /**
     * Draw the widget
     */
    void Draw() override;

    /**
     * Notify that the widget data has changed to allow the data dispatcher to execute its behaviour
     */
    void NotifyChange();

private:
    T &data_;
};

template <typename T>
inline void DataWidget<T>::Draw() {
    if (enabled_) {
        TRY_GATHER(T, data_)
        Widget::Draw();
        TRY_PROVIDE(T, data_)
    }
}

template <typename T>
inline void DataWidget<T>::NotifyChange() {
    TRY_NOTIFY_CHANGE(T)
}

}  // namespace vox::ui
