//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef lua_math_binder_hpp
#define lua_math_binder_hpp

#include <sol/sol.hpp>

namespace vox {
/**
 * Handle lua maths binding
 */
class LuaMathsBinder {
public:
    /**
     * Bind engine maths-related features to lua
     */
    static void bindMaths(sol::state& luaState);
};

}  // namespace vox
#endif /* lua_math_binder_hpp */
