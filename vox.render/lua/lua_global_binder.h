//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef lua_global_binder_hpp
#define lua_global_binder_hpp

#include <sol/sol.hpp>

namespace vox {
/**
 * Handle lua global binding
 */
class LuaGlobalsBinder {
public:
    /**
     * Bind engine global features to lua
     */
    static void bindGlobals(sol::state& luaState);
};


}
#endif /* lua_global_binder_hpp */
