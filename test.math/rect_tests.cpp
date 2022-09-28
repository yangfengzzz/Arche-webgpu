//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <gtest/gtest.h>

#include "vox.math/rect.h"

TEST(RectInt, vox_math) {
    vox::RectInt rect(10, 20, 30, 40);

    EXPECT_EQ(rect.right(), 40);
    EXPECT_EQ(rect.top(), 60);

    EXPECT_TRUE(rect.is_inside(10, 20));
    EXPECT_TRUE(rect.is_inside(39, 59));

    EXPECT_FALSE(rect.is_inside(9, 20));
    EXPECT_FALSE(rect.is_inside(10, 19));
    EXPECT_FALSE(rect.is_inside(40, 59));
    EXPECT_FALSE(rect.is_inside(39, 60));
}

TEST(RectFloat, vox_math) {
    vox::RectFloat rect(10.f, 20.f, 30.f, 40.f);

    EXPECT_FLOAT_EQ(rect.right(), 40.f);
    EXPECT_FLOAT_EQ(rect.top(), 60.f);

    EXPECT_TRUE(rect.is_inside(10.f, 20.f));
    EXPECT_TRUE(rect.is_inside(39.f, 59.f));

    EXPECT_FALSE(rect.is_inside(9.f, 20.f));
    EXPECT_FALSE(rect.is_inside(10.f, 19.f));
    EXPECT_FALSE(rect.is_inside(40.f, 59.f));
    EXPECT_FALSE(rect.is_inside(39.f, 60.f));
}
