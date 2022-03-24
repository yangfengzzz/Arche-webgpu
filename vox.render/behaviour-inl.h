//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef behaviour_inl_h
#define behaviour_inl_h

#include "behaviour.h"
#include <glog/logging.h>

namespace vox {
template<typename ...Args>
inline void Behaviour::luaCall(const std::string &functionName, Args &&...args) {
    if (_object.valid()) {
        if (_object[functionName].valid()) {
            sol::protected_function pfr = _object[functionName];
            auto pfrResult = pfr.call(_object, std::forward<Args>(args)...);
            if (!pfrResult.valid()) {
                sol::error err = pfrResult;
                LOG(ERROR) << err.what() << std::endl;;
            }
        }
    }
}

}
#endif /* behaviour_inl_h */
