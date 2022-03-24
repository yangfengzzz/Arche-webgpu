//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "lua_math_binder.h"
#include "vector2.h"
#include "vector3.h"

namespace vox {
void LuaMathsBinder::bindMaths(sol::state& luaState) {
    //MARK: - Vector2
    luaState.new_usertype<Vector2F>("Vector2",
                                    /* Constructors */
                                    sol::constructors<Vector2F(), Vector2F(float, float)>(),
                                    
                                    /* Operators */
                                    sol::meta_function::unary_minus, sol::resolve<Vector2F(const Vector2F &a)>(&operator-),
                                    sol::meta_function::addition, sol::resolve<Vector2F(const Vector2F &a, const Vector2F &b)>(&operator+),
                                    sol::meta_function::subtraction, sol::resolve<Vector2F(const Vector2F &a, const Vector2F &b)>(&operator-),
                                    sol::meta_function::multiplication, sol::resolve<Vector2F(const Vector2F &a, float b)>(&operator*),
                                    sol::meta_function::division, sol::resolve<Vector2F(const Vector2F &a, float b)>(&operator/),
                                    sol::meta_function::equal_to, &Vector2F::operator==,
                                    sol::meta_function::to_string, [](const Vector2F& target) {
        return "(" + std::to_string(target.x) + "," + std::to_string(target.y) + ")";
    },
                                    /* Variables */
                                    "x", &Vector2F::x,
                                    "y", &Vector2F::y,
                                    
                                    /* Data */
                                    "zero", []() { return zero<Vector2F>(); },
                                    
                                    /* Methods */
                                    "length", &Vector2F::length,
                                    "lengthSquared", &Vector2F::lengthSquared,
                                    "distanceTo", &Vector2F::distanceTo,
                                    "distanceSquaredTo", &Vector2F::distanceSquaredTo,
                                    "dot", &Vector2F::dot,
                                    "cross", &Vector2F::cross,
                                    "normalize", &Vector2F::normalize
                                    );
    
    //MARK: - Vector3
    luaState.new_usertype<Vector3F>("Vector3",
                                    /* Constructors */
                                    sol::constructors<Vector3F(), Vector3F(float, float, float)>(),
                                    
                                    /* Variables */
                                    "x", &Vector3F::x,
                                    "y", &Vector3F::y,
                                    "z", &Vector3F::z
                                    );
}

}
