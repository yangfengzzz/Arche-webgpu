//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.math/scalable_transform.h"

#include <gtest/gtest.h>
#include "test.math/gtest_math_helper.h"

using vox::ScalableTransform;

TEST(TransformConstant, ozz_math) {
  EXPECT_FLOAT3_EQ(ScalableTransform::identity().translation, 0.f, 0.f, 0.f);
  EXPECT_QUATERNION_EQ(ScalableTransform::identity().rotation, 0.f, 0.f, 0.f, 1.f);
  EXPECT_FLOAT3_EQ(ScalableTransform::identity().scale, 1.f, 1.f, 1.f);
}
