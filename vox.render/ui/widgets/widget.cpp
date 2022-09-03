//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/ui/widgets/widget.h"

namespace vox::ui {
uint64_t Widget::widget_id_increment_ = 0;

Widget::Widget() { widget_id_ = "##" + std::to_string(widget_id_increment_++); }

void Widget::LinkTo(const Widget &widget) { widget_id_ = widget.widget_id_; }

void Widget::Destroy() { destroyed_ = true; }

bool Widget::IsDestroyed() const { return destroyed_; }

void Widget::SetParent(WidgetContainer *parent) { parent_ = parent; }

bool Widget::HasParent() const { return parent_ != nullptr; }

WidgetContainer *Widget::Parent() { return parent_; }

void Widget::Draw() {
    if (enabled_) {
        DrawImpl();

        if (auto_execute_plugins_) ExecutePlugins();

        if (!line_break_) ImGui::SameLine();
    }
}

}  // namespace vox::ui
