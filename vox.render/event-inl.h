//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef event_inl_h
#define event_inl_h

#include "event.h"

namespace vox {
template<class... ArgTypes>
ListenerID Event<ArgTypes...>::addListener(Callback p_callback) {
    ListenerID listenerID = _availableListenerID++;
    _callbacks.emplace(listenerID, p_callback);
    return listenerID;
}

template<class... ArgTypes>
ListenerID Event<ArgTypes...>::operator+=(Callback p_callback) {
    return addListener(p_callback);
}

template<class... ArgTypes>
bool Event<ArgTypes...>::removeListener(ListenerID p_listenerID) {
    return _callbacks.erase(p_listenerID) != 0;
}

template<class... ArgTypes>
bool Event<ArgTypes...>::operator-=(ListenerID p_listenerID) {
    return removeListener(p_listenerID);
}

template<class... ArgTypes>
void Event<ArgTypes...>::removeAllListeners() {
    _callbacks.clear();
}

template<class... ArgTypes>
uint64_t Event<ArgTypes...>::listenerCount() {
    return _callbacks.size();
}

template<class... ArgTypes>
void Event<ArgTypes...>::invoke(ArgTypes... p_args) {
    for (auto const& [key, value] : _callbacks)
        value(p_args...);
}
}

#endif /* event_inl_h */
