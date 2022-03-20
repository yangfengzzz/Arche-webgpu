//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "widget.h"

namespace vox {
namespace ui {
uint64_t Widget::__WIDGET_ID_INCREMENT = 0;

Widget::Widget() {
    _widgetID = "##" + std::to_string(__WIDGET_ID_INCREMENT++);
}

void Widget::linkTo(const Widget &p_widget) {
    _widgetID = p_widget._widgetID;
}

void Widget::destroy() {
    _destroyed = true;
}

bool Widget::isDestroyed() const {
    return _destroyed;
}

void Widget::setParent(WidgetContainer *p_parent) {
    _parent = p_parent;
}

bool Widget::hasParent() const {
    return _parent != nullptr;
}

WidgetContainer *Widget::parent() {
    return _parent;
}

void Widget::draw() {
    if (enabled) {
        _draw_Impl();
        
        if (_autoExecutePlugins)
            executePlugins();
        
        if (!lineBreak)
            ImGui::SameLine();
    }
}


}
}
