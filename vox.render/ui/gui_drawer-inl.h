//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/ui/gui_drawer.h"

namespace vox::ui {
template <typename T>
inline ImGuiDataType_ GuiDrawer::GetDataType() {
    if constexpr (std::is_same<T, float>::value)
        return ImGuiDataType_Float;
    else if constexpr (std::is_same<T, double>::value)
        return ImGuiDataType_Double;
    else if constexpr (std::is_same<T, uint8_t>::value)
        return ImGuiDataType_U8;
    else if constexpr (std::is_same<T, uint16_t>::value)
        return ImGuiDataType_U16;
    else if constexpr (std::is_same<T, uint32_t>::value)
        return ImGuiDataType_U32;
    else if constexpr (std::is_same<T, uint64_t>::value)
        return ImGuiDataType_U64;
    else if constexpr (std::is_same<T, int8_t>::value)
        return ImGuiDataType_S8;
    else if constexpr (std::is_same<T, int16_t>::value)
        return ImGuiDataType_S16;
    else if constexpr (std::is_same<T, int32_t>::value)
        return ImGuiDataType_S32;
    else if constexpr (std::is_same<T, int64_t>::value)
        return ImGuiDataType_S64;
}

template <typename T>
inline std::string GuiDrawer::GetFormat() {
    if constexpr (std::is_same<T, double>::value)
        return "%.5f";
    else if constexpr (std::is_same<T, float>::value)
        return "%.3f";
    else
        return "%d";
}

template <typename T>
inline void GuiDrawer::DrawScalar(WidgetContainer &root, const std::string &name, T &data, float step, T min, T max) {
    static_assert(std::is_scalar<T>::value, "T must be a scalar");

    CreateTitle(root, name);
    auto &widget = root.CreateWidget<DragSingleScalar<T>>(GetDataType<T>(), min, max, data, step, "", GetFormat<T>());
    auto &dispatcher = widget.template AddPlugin<DataDispatcher<T>>();
    dispatcher.RegisterReference(data);
}

template <typename T>
inline void GuiDrawer::DrawScalar(WidgetContainer &root,
                                  const std::string &name,
                                  std::function<T(void)> gatherer,
                                  std::function<void(T)> provider,
                                  float step,
                                  T min,
                                  T max) {
    static_assert(std::is_scalar<T>::value, "T must be a scalar");

    CreateTitle(root, name);
    auto &widget = root.CreateWidget<DragSingleScalar<T>>(GetDataType<T>(), min, max, static_cast<T>(0), step, "",
                                                          GetFormat<T>());
    auto &dispatcher = widget.template AddPlugin<DataDispatcher<T>>();
    dispatcher.RegisterGatherer(gatherer);
    dispatcher.RegisterProvider(provider);
}

}  // namespace vox::ui
