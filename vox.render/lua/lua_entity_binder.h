//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef lua_entity_binder_hpp
#define lua_entity_binder_hpp

#include <sol/sol.hpp>

namespace vox {
/**
 * Handle entity binding
 */
class LuaEntityBinder {
public:
    /**
     * Bind engine entity-related features to lua
     */
    static void bindEntity(sol::state& luaState);
};

}  // namespace vox
#endif /* lua_entity_binder_hpp */
