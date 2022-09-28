//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

static const float kFloatNearTolerance = 1e-5f;
static const float kFloatNearEstTolerance = 1e-3f;

// Implements "float near" test as a function. Avoids overloading compiler
// optimizer when too much EXPECT_NEAR are used in a single compilation unit.
inline void ExpectFloatNear(float _a, float _b, float _tol = kFloatNearTolerance) { EXPECT_NEAR(_a, _b, _tol); }

// Implements "int equality" test as a function. Avoids overloading compiler
// optimizer when too much EXPECT_TRUE are used in a single compilation unit.
inline void ExpectIntEq(int _a, int _b) { EXPECT_EQ(_a, _b); }

// Implements "bool equality" test as a function. Avoids overloading compiler
// optimizer when too much EXPECT_TRUE are used in a single compilation unit.
inline void ExpectTrue(bool _b) { EXPECT_TRUE(_b); }

// Macro for testing floats, dedicated to estimated functions with a lower
// precision.
#define EXPECT_FLOAT_EQ_EST(_expected, _x) EXPECT_NEAR(_expected, _x, kFloatNearEstTolerance)

// Macro for testing vox::Vector4F members with x, y, z, w float values,
// using EXPECT_FLOAT_EQ internally.
#define EXPECT_FLOAT4_EQ(_expected, _x, _y, _z, _w) \
                                                    \
    do {                                            \
        SCOPED_TRACE("");                           \
        const vox::Vector4F expected(_expected);    \
        ExpectFloatNear(expected.x, _x);            \
        ExpectFloatNear(expected.y, _y);            \
        ExpectFloatNear(expected.z, _z);            \
        ExpectFloatNear(expected.w, _w);            \
                                                    \
    } while (void(0), 0)

// Macro for testing vox::Vector3F members with x, y, z float values,
// using EXPECT_FLOAT_EQ internally.
#define EXPECT_FLOAT3_EQ(_expected, _x, _y, _z)  \
                                                 \
    do {                                         \
        SCOPED_TRACE("");                        \
        const vox::Vector3F expected(_expected); \
        ExpectFloatNear(expected.x, _x);         \
        ExpectFloatNear(expected.y, _y);         \
        ExpectFloatNear(expected.z, _z);         \
                                                 \
    } while (void(0), 0)

// Macro for testing vox::Vector3F members with x, y, z float values,
// using EXPECT_FLOAT_EQ internally.
#define EXPECT_POINT3_EQ(_expected, _x, _y, _z)  \
                                                 \
    do {                                         \
        SCOPED_TRACE("");                        \
        const vox::Point3F expected(_expected); \
        ExpectFloatNear(expected.x, _x);         \
        ExpectFloatNear(expected.y, _y);         \
        ExpectFloatNear(expected.z, _z);         \
                                                 \
    } while (void(0), 0)

// Macro for testing vox::Vector2F members with x, y float values,
// using EXPECT_NEAR internally.
#define EXPECT_FLOAT2_EQ(_expected, _x, _y)      \
                                                 \
    do {                                         \
        SCOPED_TRACE("");                        \
        const vox::Vector2F expected(_expected); \
        ExpectFloatNear(expected.x, _x);         \
        ExpectFloatNear(expected.y, _y);         \
                                                 \
    } while (void(0), 0)

// Macro for testing vox::Quaternion members with x, y, z, w float value.
#define EXPECT_QUATERNION_EQ(_expected, _x, _y, _z, _w) \
                                                        \
    do {                                                \
        SCOPED_TRACE("");                               \
        const vox::QuaternionF expected(_expected);     \
        ExpectFloatNear(expected.x, _x);                \
        ExpectFloatNear(expected.y, _y);                \
        ExpectFloatNear(expected.z, _z);                \
        ExpectFloatNear(expected.w, _w);                \
                                                        \
    } while (void(0), 0)