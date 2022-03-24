//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "lua_math_binder.h"
#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "point3.h"
#include "matrix3x3.h"
#include "matrix4x4.h"
#include "quaternion.h"

namespace vox {
void LuaMathsBinder::bindMaths(sol::state& luaState) {
    //MARK: - Vector2
    luaState.new_usertype<Vector2F>("Vector2F",
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
    luaState.new_usertype<Vector3F>("Vector3F",
                                    /* Constructors */
                                    sol::constructors<Vector3F(), Vector3F(float, float, float)>(),
                                    
                                    /* Operators */
                                    sol::meta_function::unary_minus, sol::resolve<Vector3F(const Vector3F &a)>(&operator-),
                                    sol::meta_function::addition, sol::resolve<Vector3F(const Vector3F &a, const Vector3F &b)>(&operator+),
                                    sol::meta_function::subtraction, sol::resolve<Vector3F(const Vector3F &a, const Vector3F &b)>(&operator-),
                                    sol::meta_function::multiplication, sol::resolve<Vector3F(const Vector3F &a, float b)>(&operator*),
                                    sol::meta_function::division, sol::resolve<Vector3F(const Vector3F &a, float b)>(&operator/),
                                    sol::meta_function::equal_to, &Vector3F::operator==,
                                    sol::meta_function::to_string, [](const Vector3F& target) {
        return "(" + std::to_string(target.x) + "," + std::to_string(target.y) + "," + std::to_string(target.z) + ")";
    },
                                    /* Variables */
                                    "x", &Vector3F::x,
                                    "y", &Vector3F::y,
                                    "z", &Vector3F::z,
                                    
                                    /* Data */
                                    "zero", []() { return zero<Vector3F>(); },
                                    
                                    /* Methods */
                                    "length", &Vector3F::length,
                                    "lengthSquared", &Vector3F::lengthSquared,
                                    "distanceTo", &Vector3F::distanceTo,
                                    "distanceSquaredTo", &Vector3F::distanceSquaredTo,
                                    "dot", &Vector3F::dot,
                                    "cross", &Vector3F::cross,
                                    "normalize", &Vector3F::normalize
                                    );
    
    //MARK: - Vector4
    luaState.new_usertype<Vector4F>("Vector4F",
                                    /* Constructors */
                                    sol::constructors<Vector4F(), Vector4F(float, float, float, float)>(),
                                    
                                    /* Operators */
                                    sol::meta_function::unary_minus, sol::resolve<Vector4F(const Vector4F &a)>(&operator-),
                                    sol::meta_function::addition, sol::resolve<Vector4F(const Vector4F &a, const Vector4F &b)>(&operator+),
                                    sol::meta_function::subtraction, sol::resolve<Vector4F(const Vector4F &a, const Vector4F &b)>(&operator-),
                                    sol::meta_function::multiplication, sol::resolve<Vector4F(const Vector4F &a, float b)>(&operator*),
                                    sol::meta_function::division, sol::resolve<Vector4F(const Vector4F &a, float b)>(&operator/),
                                    sol::meta_function::equal_to, &Vector4F::operator==,
                                    sol::meta_function::to_string, [](const Vector4F& target) {
        return "(" + std::to_string(target.x) + "," + std::to_string(target.y)
        + "," + std::to_string(target.z) + "," + std::to_string(target.w) + ")";
    },
                                    /* Variables */
                                    "x", &Vector4F::x,
                                    "y", &Vector4F::y,
                                    "z", &Vector4F::z,
                                    "w", &Vector4F::w,
                                    
                                    /* Data */
                                    "zero", []() { return zero<Vector4F>(); },
                                    
                                    /* Methods */
                                    "length", &Vector4F::length,
                                    "lengthSquared", &Vector4F::lengthSquared,
                                    "distanceTo", &Vector4F::distanceTo,
                                    "distanceSquaredTo", &Vector4F::distanceSquaredTo,
                                    "dot", &Vector4F::dot,
                                    "normalize", &Vector4F::normalize
                                    );
    
    //MARK: - Point3
    luaState.new_usertype<Point3F>("Point3F",
                                   /* Constructors */
                                   sol::constructors<Point3F(), Point3F(float, float, float)>(),
                                   
                                   /* Operators */
                                   sol::meta_function::unary_minus, sol::resolve<Point3F(const Point3F &a)>(&operator-),
                                   sol::meta_function::addition, sol::resolve<Vector3F(const Point3F &a, const Point3F &b)>(&operator+),
                                   sol::meta_function::subtraction, sol::resolve<Vector3F(const Point3F &a, const Point3F &b)>(&operator-),
                                   sol::meta_function::multiplication, sol::resolve<Point3F(const Point3F &a, float b)>(&operator*),
                                   sol::meta_function::division, sol::resolve<Point3F(const Point3F &a, float b)>(&operator/),
                                   sol::meta_function::equal_to, &Point3F::operator==,
                                   sol::meta_function::to_string, [](const Point3F& target) {
        return "(" + std::to_string(target.x) + "," + std::to_string(target.y) + "," + std::to_string(target.z) + ")";
    },
                                   /* Variables */
                                   "x", &Point3F::x,
                                   "y", &Point3F::y,
                                   "z", &Point3F::z,
                                   
                                   /* Methods */
                                   "distanceTo", &Point3F::distanceTo,
                                   "distanceSquaredTo", &Point3F::distanceSquaredTo,
                                   "dot", &Point3F::dot,
                                   "cross", &Point3F::cross
                                   );
    
    //MARK: - Matrix3x3
    luaState.new_usertype<Matrix3x3F>("Matrix3x3F",
                                      /* Constructors */
                                      sol::constructors<Matrix3x3F(), Matrix3x3F(float),
                                      Matrix3x3F(float, float, float, float, float, float, float, float, float)>(),
                                      
                                      /* Operators */
                                      sol::meta_function::unary_minus, sol::resolve<Matrix3x3F(const Matrix3x3F &a)>(&operator-),
                                      sol::meta_function::addition, sol::overload(sol::resolve<Matrix3x3F(const Matrix3x3F &a, const Matrix3x3F &b)>(&operator+),
                                                                                  sol::resolve<Matrix3x3F(const Matrix3x3F &a, float b)>(&operator+),
                                                                                  sol::resolve<Matrix3x3F(float a, const Matrix3x3F &b)>(&operator+)),
                                      sol::meta_function::subtraction, sol::overload(sol::resolve<Matrix3x3F(const Matrix3x3F &a, const Matrix3x3F &b)>(&operator-),
                                                                                     sol::resolve<Matrix3x3F(const Matrix3x3F &a, float b)>(&operator-),
                                                                                     sol::resolve<Matrix3x3F(float a, const Matrix3x3F &b)>(&operator-)),
                                      sol::meta_function::multiplication, sol::overload(sol::resolve<Matrix3x3F(const Matrix3x3F &a, float b)>(&operator*),
                                                                                        sol::resolve<Matrix3x3F(float a, const Matrix3x3F &b)>(&operator*),
                                                                                        sol::resolve<Vector3F(const Matrix3x3F &a, const Vector3F &b)>(&operator*),
                                                                                        sol::resolve<Point3F(const Matrix3x3F &a, const Point3F &b)>(&operator*),
                                                                                        sol::resolve<Matrix3x3F(const Matrix3x3F &a, const Matrix3x3F &b)>(&operator*)),
                                      sol::meta_function::division, sol::overload(sol::resolve<Matrix3x3F(const Matrix3x3F &a, float b)>(&operator/),
                                                                                  sol::resolve<Matrix3x3F(float a, const Matrix3x3F &b)>(&operator/)),
                                      sol::meta_function::equal_to, &Matrix3x3F::operator==,
                                      
                                      
                                      /* Data */
                                      "makeZero", &Matrix3x3F::makeZero,
                                      "makeIdentity", &Matrix3x3F::makeIdentity,
                                      "makeScaleMatrix", sol::overload(sol::resolve<Matrix3x3F(float a, float b, float c)>(&Matrix3x3F::makeScaleMatrix),
                                                                       sol::resolve<Matrix3x3F(const Vector3F &b)>(&Matrix3x3F::makeScaleMatrix)),
                                      "makeRotationMatrix", &Matrix3x3F::makeRotationMatrix
                                      
                                      );
    
    //MARK: - Matrix4x4
    luaState.new_usertype<Matrix4x4F>("Matrix4x4F",
                                      /* Constructors */
                                      sol::constructors<Matrix4x4F(), Matrix4x4F(float),
                                      Matrix3x3F(float, float, float, float,
                                                 float, float, float, float,
                                                 float, float, float, float,
                                                 float, float, float, float)>(),
                                      
                                      /* Operators */
                                      sol::meta_function::unary_minus, sol::resolve<Matrix4x4F(const Matrix4x4F &a)>(&operator-),
                                      sol::meta_function::addition, sol::overload(sol::resolve<Matrix4x4F(const Matrix4x4F &a, const Matrix4x4F &b)>(&operator+),
                                                                                  sol::resolve<Matrix4x4F(const Matrix4x4F &a, float b)>(&operator+),
                                                                                  sol::resolve<Matrix4x4F(float a, const Matrix4x4F &b)>(&operator+)),
                                      sol::meta_function::subtraction, sol::overload(sol::resolve<Matrix4x4F(const Matrix4x4F &a, const Matrix4x4F &b)>(&operator-),
                                                                                     sol::resolve<Matrix4x4F(const Matrix4x4F &a, float b)>(&operator-),
                                                                                     sol::resolve<Matrix4x4F(float a, const Matrix4x4F &b)>(&operator-)),
                                      sol::meta_function::multiplication, sol::overload(sol::resolve<Matrix4x4F(const Matrix4x4F &a, float b)>(&operator*),
                                                                                        sol::resolve<Matrix4x4F(float a, const Matrix4x4F &b)>(&operator*),
                                                                                        sol::resolve<Vector4F(const Matrix4x4F &a, const Vector4F &b)>(&operator*),
                                                                                        sol::resolve<Matrix4x4F(const Matrix4x4F &a, const Matrix4x4F &b)>(&operator*)),
                                      sol::meta_function::division, sol::overload(sol::resolve<Matrix4x4F(const Matrix4x4F &a, float b)>(&operator/),
                                                                                  sol::resolve<Matrix4x4F(float a, const Matrix4x4F &b)>(&operator/)),
                                      sol::meta_function::equal_to, &Matrix4x4F::operator==,
                                      
                                      
                                      /* Data */
                                      "makeZero", &Matrix4x4F::makeZero,
                                      "makeIdentity", &Matrix4x4F::makeIdentity
                                      );
    
    //MARK: - Quaternion
    luaState.new_usertype<QuaternionF>("QuaternionF",
                                       /* Constructors */
                                       sol::constructors<QuaternionF(),
                                       QuaternionF(float, float, float, float),
                                       QuaternionF(const Vector3F&, float),
                                       QuaternionF(const Vector3F&, const Vector3F&),
                                       QuaternionF(const Vector3F&, const Vector3F&, const Vector3F&)>(),
                                       
                                       /* Operators */
                                       sol::meta_function::addition, sol::resolve<QuaternionF(const QuaternionF &a, const QuaternionF &b)>(&operator+),
                                       sol::meta_function::multiplication, sol::resolve<QuaternionF(const QuaternionF &a, float b)>(&operator*),
                                       sol::meta_function::equal_to, &QuaternionF::operator==,
                                       sol::meta_function::to_string, [](const QuaternionF& target) {
           return "(" + std::to_string(target.x) + "," + std::to_string(target.y)
           + "," + std::to_string(target.z) + "," + std::to_string(target.w) + ")";
       },
                                       /* Variables */
                                       "x", &QuaternionF::x,
                                       "y", &QuaternionF::y,
                                       "z", &QuaternionF::z,
                                       "w", &QuaternionF::w,
                                       
                                       /* Data */
                                       "makeIdentity", &QuaternionF::makeIdentity,
                                       "makeRotationEuler", &QuaternionF::makeRotationEuler,
                                       "makeRotationYawPitchRoll", &QuaternionF::makeRotationYawPitchRoll,
                                       "makeRotationX", &QuaternionF::makeRotationX,
                                       "makeRotationY", &QuaternionF::makeRotationY,
                                       "makeRotationZ", &QuaternionF::makeRotationZ,
                                       
                                       /* Methods */
                                       "length", &QuaternionF::length,
                                       "lengthSquared", &QuaternionF::lengthSquared,
                                       "dot", &QuaternionF::dot,
                                       "normalize", &QuaternionF::normalize,
                                       "slerp", sol::resolve<QuaternionF(const QuaternionF &, const QuaternionF &, float)>(&slerp)
                                       );
}

}
