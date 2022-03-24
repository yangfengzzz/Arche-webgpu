//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef lua_component_binder_hpp
#define lua_component_binder_hpp

#include <sol/sol.hpp>

namespace vox {
/**
 * Handle lua components binding
 */
class LuaComponentBinder {
public:
    /**
     * Bind engine component-related features to lua
     */
    static void bindComponent(sol::state& luaState);
};


}
#endif /* lua_component_binder_hpp */
