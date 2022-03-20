//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef event_h
#define event_h

#include <functional>
#include <unordered_map>

namespace vox {
/**
 * The ID of a listener (Registered callback).
 * This value is needed to remove a listener from an event
 */
using ListenerID = uint64_t;

/**
 * A simple event that contains a set of function callbacks. These functions will be called on invoke
 */
template<class... ArgTypes>
class Event {
public:
    /**
     * Simple shortcut for a generic function without return value
     */
    using Callback = std::function<void(ArgTypes...)>;
    
    /**
     * Add a function callback to this event
     * Also return the ID of the new listener (You should store the returned ID if you want to remove the listener later)
     * @param p_callback p_callback
     */
    ListenerID addListener(Callback p_callback);
    
    /**
     * Add a function callback to this event
     * Also return the ID of the new listener (You should store the returned ID if you want to remove the listener later)
     * @param p_callback p_callback
     */
    ListenerID operator+=(Callback p_callback);
    
    /**
     * Remove a function callback to this event using a Listener (Created when calling AddListener)
     * @param p_listenerID p_listenerID
     */
    bool removeListener(ListenerID p_listenerID);
    
    /**
     * Remove a function callback to this event using a Listener (Created when calling AddListener)
     * @param p_listenerID p_listenerID
     */
    bool operator-=(ListenerID p_listenerID);
    
    /**
     * Remove every listeners to this event
     */
    void removeAllListeners();
    
    /**
     * Return the number of callback registered
     */
    uint64_t listenerCount();
    
    /**
     * Call every callbacks attached to this event
     * @param p_args (Variadic)
     */
    void invoke(ArgTypes... p_args);
    
private:
    std::unordered_map<ListenerID, Callback> _callbacks;
    ListenerID _availableListenerID = 0;
};
}

#include "event-inl.h"

#endif /* event_h */
