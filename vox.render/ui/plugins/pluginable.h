//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef pluginable_h
#define pluginable_h

#include "plugin.h"
#include <vector>

namespace vox {
namespace ui {
/**
 * Inherit from this class to make your class "Pluginable" (Able to have plugins)
 */
class Pluginable {
public:
    /**
     * Destructor (Destroys every plugins)
     */
    ~Pluginable() {
        removeAllPlugins();
    }
    
    /**
     * Add a plugin
     * @param p_args p_args
     */
    template<typename T, typename... Args>
    T &addPlugin(Args &&... p_args) {
        static_assert(std::is_base_of<Plugin, T>::value, "T should derive from IPlugin");
        
        T *newPlugin = new T(std::forward<Args>(p_args)...);
        _plugins.push_back(newPlugin);
        return *newPlugin;
    }
    
    /**
     * Returns the plugin of the given type, or nullptr if not found
     */
    template<typename T>
    T *getPlugin() {
        static_assert(std::is_base_of<Plugin, T>::value, "T should derive from IPlugin");
        
        for (auto it = _plugins.begin(); it != _plugins.end(); ++it) {
            T *result = dynamic_cast<T *>(*it);
            if (result)
                return result;
        }
        
        return nullptr;
    }
    
    /**
     * Execute every plugins
     */
    void executePlugins() {
        for (auto &plugin: _plugins)
            plugin->execute();
    }
    
    /**
     * Remove every plugins
     */
    void removeAllPlugins() {
        for (auto &plugin: _plugins)
            delete plugin;
        
        _plugins.clear();
    }
    
private:
    std::vector<Plugin *> _plugins;
};


}
}
#endif /* pluginable_h */
