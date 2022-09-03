//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>

#include "vox.render/ui/plugins/plugin.h"

namespace vox::ui {
/**
 * Inherit from this class to make your class "Pluginable" (Able to have plugins)
 */
class Pluginable {
public:
    /**
     * Destructor (Destroys every plugins)
     */
    ~Pluginable() { RemoveAllPlugins(); }

    /**
     * Add a plugin
     */
    template <typename T, typename... Args>
    T &AddPlugin(Args &&...args) {
        static_assert(std::is_base_of<Plugin, T>::value, "T should derive from IPlugin");

        T *new_plugin = new T(std::forward<Args>(args)...);
        plugins_.push_back(new_plugin);
        return *new_plugin;
    }

    /**
     * Returns the plugin of the given type, or nullptr if not found
     */
    template <typename T>
    T *GetPlugin() {
        static_assert(std::is_base_of<Plugin, T>::value, "T should derive from IPlugin");

        for (auto &plugin : plugins_) {
            T *result = dynamic_cast<T *>(plugin);
            if (result) return result;
        }

        return nullptr;
    }

    /**
     * Execute every plugins
     */
    void ExecutePlugins() {
        for (auto &plugin : plugins_) plugin->Execute();
    }

    /**
     * Remove every plugins
     */
    void RemoveAllPlugins() {
        for (auto &plugin : plugins_) delete plugin;

        plugins_.clear();
    }

private:
    std::vector<Plugin *> plugins_;
};

}  // namespace vox::ui
