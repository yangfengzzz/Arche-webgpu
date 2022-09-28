//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gtest/gtest.h"
#include "test.simd_math/gtest_math_helper.h"
#include "vox.simd_math/soa_transform.h"

using vox::math::SoaTransform;

TEST(SoaTransformConstant, vox_soa_math) {
    EXPECT_SOAFLOAT3_EQ(SoaTransform::identity().translation, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
                        0.f);
    EXPECT_SOAQUATERNION_EQ(SoaTransform::identity().rotation, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
                            0.f, 1.f, 1.f, 1.f, 1.f);
    EXPECT_SOAFLOAT3_EQ(SoaTransform::identity().scale, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f);
}
