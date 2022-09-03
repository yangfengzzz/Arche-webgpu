//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef lua_binder_hpp
#define lua_binder_hpp

#include <sol/sol.hpp>

namespace vox {
/**
 * Handle lua binding
 */
class LuaBinder {
public:
    /**
     * Bind engine features to lua
     */
    static void callBinders(sol::state& luaState);
};

}  // namespace vox
#endif /* lua_binder_hpp */
