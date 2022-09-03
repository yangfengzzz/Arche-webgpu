//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>

#include "vox.render/ui/plugins/plugin.h"

namespace vox::ui {
#define TRY_GATHER(type, output) \
    if (auto plugin = GetPlugin<DataDispatcher<type>>(); plugin) output = plugin->Gather();
#define TRY_PROVIDE(type, output) \
    if (auto plugin = GetPlugin<DataDispatcher<type>>(); plugin) plugin->Provide(output);
#define TRY_NOTIFY_CHANGE(type) \
    if (auto plugin = GetPlugin<DataDispatcher<type>>(); plugin) plugin->NotifyChange();

/**
 * Plugin that allow automatic data update of any DataWidget
 */
template <typename T>
class DataDispatcher : public Plugin {
public:
    /**
     * Register a reference
     */
    void RegisterReference(T &reference) { data_pointer_ = &reference; }

    /**
     * Register a provider (Which function should be called when the widget data is modified)
     */
    void RegisterProvider(std::function<void(T)> provider) { provider_ = provider; }

    /**
     * Register a gather (Which function should be called when the widget data needs to be updated)
     */
    void RegisterGatherer(std::function<T(void)> gatherer) { gatherer_ = gatherer; }

    /**
     * Provide data to the dispatcher
     */
    void Provide(T data) {
        if (value_changed_) {
            if (data_pointer_)
                *data_pointer_ = data;
            else
                provider_(data);

            value_changed_ = false;
        }
    }

    /**
     * Notify that a change occurred
     */
    void NotifyChange() { value_changed_ = true; }

    /**
     * Returns the data from the dispatcher
     */
    T Gather() { return data_pointer_ ? *data_pointer_ : gatherer_(); }

    /**
     * Execute the data dispatcher behaviour (No effect)
     */
    void Execute() override {}

private:
    bool value_changed_ = false;
    T *data_pointer_ = nullptr;
    std::function<void(T)> provider_;
    std::function<T(void)> gatherer_;
};

}  // namespace vox::ui
