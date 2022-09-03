//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>
#include <unordered_map>

namespace vox {
/**
 * The ID of a listener (Registered callback).
 * This value is needed to remove a listener from an event
 */
using ListenerId = uint64_t;

/**
 * A simple event that contains a set of function callbacks. These functions will be called on invoke
 */
template <class... ArgTypes>
class Event {
public:
    /**
     * Simple shortcut for a generic function without return value
     */
    using Callback = std::function<void(ArgTypes...)>;

    /**
     * Add a function callback to this event
     * Also return the ID of the new listener (You should store the returned ID if you want to remove the listener
     * later)
     * @param p_callback p_callback
     */
    ListenerId AddListener(Callback p_callback);

    /**
     * Add a function callback to this event
     * Also return the ID of the new listener (You should store the returned ID if you want to remove the listener
     * later)
     * @param p_callback p_callback
     */
    ListenerId operator+=(Callback p_callback);

    /**
     * Remove a function callback to this event using a Listener (Created when calling AddListener)
     * @param p_listener_id p_listenerID
     */
    bool RemoveListener(ListenerId p_listener_id);

    /**
     * Remove a function callback to this event using a Listener (Created when calling AddListener)
     * @param p_listener_id p_listenerID
     */
    bool operator-=(ListenerId p_listener_id);

    /**
     * Remove every listeners to this event
     */
    void RemoveAllListeners();

    /**
     * Return the number of callback registered
     */
    uint64_t ListenerCount();

    /**
     * Call every callbacks attached to this event
     * @param p_args (Variadic)
     */
    void Invoke(ArgTypes... p_args);

private:
    std::unordered_map<ListenerId, Callback> callbacks_;
    ListenerId available_listener_id_ = 0;
};
}  // namespace vox

#include "vox.render/event-inl.h"