//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/widgets/widget.h"

namespace vox::ui {
/**
 * Defines how the memory should be managed
 */
enum class MemoryMode { INTERNAL_MANAGEMENT, EXTERNAL_MANAGEMENT };

/**
 * The base class for any widget container
 */
class WidgetContainer {
public:
    /**
     * Remove a widget from the container
     */
    void RemoveWidget(Widget &widget);

    /**
     * Remove all widgets from the container
     */
    void RemoveAllWidgets();

    /**
     * Consider a widget
     */
    void ConsiderWidget(Widget &widget, bool manage_memory = true);

    /**
     * Unconsider a widget
     */
    void UnconsiderWidget(Widget &widget);

    /**
     * Collect garbage by removing widgets marked as "Destroyed"
     */
    void CollectGarbage();

    /**
     * Draw every widgets
     */
    void DrawWidgets();

    /**
     * Allow the user to reverse the draw order of this widget container
     */
    void ReverseDrawOrder(bool reversed = true);

    /**
     * Create a widget
     */
    template <typename T, typename... Args>
    T &CreateWidget(Args &&...args) {
        widgets_.emplace_back(new T(args...), MemoryMode::INTERNAL_MANAGEMENT);
        T &instance = *reinterpret_cast<T *>(widgets_.back().first);
        instance.SetParent(this);
        return instance;
    }

    /**
     * Returns the widgets and their memory management mode
     */
    std::vector<std::pair<Widget *, MemoryMode>> &Widgets();

protected:
    std::vector<std::pair<Widget *, MemoryMode>> widgets_{};
    bool reversed_draw_order_ = false;
};

}  // namespace vox::ui
