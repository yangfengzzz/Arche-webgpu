//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "lua_binder.h"

#include "lua/lua_component_binder.h"
#include "lua/lua_entity_binder.h"
#include "lua/lua_global_binder.h"
#include "lua/lua_math_binder.h"

namespace vox {
void LuaBinder::callBinders(sol::state& luaState) {
    LuaMathsBinder::bindMaths(luaState);
    LuaEntityBinder::bindEntity(luaState);
    LuaComponentBinder::bindComponent(luaState);
    LuaGlobalsBinder::bindGlobals(luaState);
}

}  // namespace vox
