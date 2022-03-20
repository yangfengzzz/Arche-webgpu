//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef data_dispatcher_h
#define data_dispatcher_h

#include <functional>

#include "ui/plugins/plugin.h"

namespace vox {
namespace ui {
#define TRY_GATHER(type, output)   if (auto plugin = getPlugin<DataDispatcher<type>>(); plugin) output = plugin->gather();
#define TRY_PROVIDE(type, output)  if (auto plugin = getPlugin<DataDispatcher<type>>(); plugin) plugin->provide(output);
#define TRY_NOTIFY_CHANGE(type)    if (auto plugin = getPlugin<DataDispatcher<type>>(); plugin) plugin->notifyChange();

/**
 * Plugin that allow automatic data update of any DataWidget
 */
template<typename T>
class DataDispatcher : public Plugin {
public:
    /**
     * Register a reference
     * @param p_reference p_reference
     */
    void registerReference(T &p_reference) {
        _dataPointer = &p_reference;
    }
    
    /**
     * Register a provider (Which function should be called when the widget data is modified)
     * @param p_provider p_provider
     */
    void registerProvider(std::function<void(T)> p_provider) {
        _provider = p_provider;
    }
    
    /**
     * Register a gather (Which function should be called when the widget data needs to be updated)
     * @param p_gatherer p_gatherer
     */
    void registerGatherer(std::function<T(void)> p_gatherer) {
        _gatherer = p_gatherer;
    }
    
    /**
     * Provide data to the dispatcher
     * @param p_data p_data
     */
    void provide(T p_data) {
        if (_valueChanged) {
            if (_dataPointer)
                *_dataPointer = p_data;
            else
                _provider(p_data);
            
            _valueChanged = false;
        }
    }
    
    /**
     * Notify that a change occured
     */
    void notifyChange() {
        _valueChanged = true;
    }
    
    /**
     * Returns the data from the dispatcher
     */
    T gather() {
        return _dataPointer ? *_dataPointer : _gatherer();
    }
    
    /**
     * Execute the data dispatcher behaviour (No effect)
     */
    virtual void execute() override {
    }
    
private:
    bool _valueChanged = false;
    T *_dataPointer = nullptr;
    std::function<void(T)> _provider;
    std::function<T(void)> _gatherer;
};


}
}
#endif /* data_dispatcher_h */
