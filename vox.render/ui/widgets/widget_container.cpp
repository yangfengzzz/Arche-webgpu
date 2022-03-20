//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "widget_container.h"
#include <algorithm>

namespace vox {
namespace ui {
void WidgetContainer::removeWidget(Widget &p_widget) {
    auto found = std::find_if(_widgets.begin(), _widgets.end(), [&p_widget](std::pair<Widget *, MemoryMode> &p_pair) {
        return p_pair.first == &p_widget;
    });
    
    if (found != _widgets.end()) {
        if (found->second == MemoryMode::INTERNAL_MANAGEMENT)
            delete found->first;
        
        _widgets.erase(found);
    }
}

void WidgetContainer::removeAllWidgets() {
    std::for_each(_widgets.begin(), _widgets.end(), [](auto &pair) {
        if (pair.second == MemoryMode::INTERNAL_MANAGEMENT)
            delete pair.first;
    });
    
    _widgets.clear();
}

void WidgetContainer::considerWidget(Widget &p_widget, bool p_manageMemory) {
    _widgets.emplace_back(std::make_pair(&p_widget, p_manageMemory ? MemoryMode::INTERNAL_MANAGEMENT : MemoryMode::EXTERNAL_MANAGEMENT));
    p_widget.setParent(this);
}

void WidgetContainer::unconsiderWidget(Widget &p_widget) {
    auto found = std::find_if(_widgets.begin(), _widgets.end(), [&p_widget](std::pair<Widget *, MemoryMode> &p_pair) {
        return p_pair.first == &p_widget;
    });
    
    if (found != _widgets.end()) {
        p_widget.setParent(nullptr);
        _widgets.erase(found);
    }
}

void WidgetContainer::collectGarbages() {
    _widgets.erase(std::remove_if(_widgets.begin(), _widgets.end(), [](std::pair<Widget *, MemoryMode> &p_item) {
        bool toDestroy = p_item.first && p_item.first->isDestroyed();
        
        if (toDestroy && p_item.second == MemoryMode::INTERNAL_MANAGEMENT)
            delete p_item.first;
        
        return toDestroy;
    }), _widgets.end());
}

void WidgetContainer::drawWidgets() {
    collectGarbages();
    
    if (_reversedDrawOrder) {
        for (auto it = _widgets.crbegin(); it != _widgets.crend(); ++it)
            it->first->draw();
    } else {
        for (const auto &widget: _widgets)
            widget.first->draw();
    }
}

void WidgetContainer::reverseDrawOrder(const bool reversed) {
    _reversedDrawOrder = reversed;
}

std::vector<std::pair<Widget *, MemoryMode>> &WidgetContainer::widgets() {
    return _widgets;
}


}
}
