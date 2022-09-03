// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include <gtest/gtest.h>

#include "vox.geometry/plane2.h"

using namespace vox;

TEST(Plane2, Builder) {
    Plane2 plane = Plane2::builder().withNormal({1, 0}).withPoint({2, 3}).build();

    EXPECT_EQ(Vector2D(1, 0), plane.normal);
    EXPECT_EQ(Point2D(2, 3), plane.point);
}
