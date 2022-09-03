//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/event.h"

namespace vox {
template <class... ArgTypes>
ListenerId Event<ArgTypes...>::AddListener(Callback p_callback) {
    ListenerId listener_id = available_listener_id_++;
    callbacks_.emplace(listener_id, p_callback);
    return listener_id;
}

template <class... ArgTypes>
ListenerId Event<ArgTypes...>::operator+=(Callback p_callback) {
    return AddListener(p_callback);
}

template <class... ArgTypes>
bool Event<ArgTypes...>::RemoveListener(ListenerId p_listener_id) {
    return callbacks_.erase(p_listener_id) != 0;
}

template <class... ArgTypes>
bool Event<ArgTypes...>::operator-=(ListenerId p_listener_id) {
    return RemoveListener(p_listener_id);
}

template <class... ArgTypes>
void Event<ArgTypes...>::RemoveAllListeners() {
    callbacks_.clear();
}

template <class... ArgTypes>
uint64_t Event<ArgTypes...>::ListenerCount() {
    return callbacks_.size();
}

template <class... ArgTypes>
void Event<ArgTypes...>::Invoke(ArgTypes... p_args) {
    for (auto const& [kEy, kValue] : callbacks_) kValue(p_args...);
}
}  // namespace vox