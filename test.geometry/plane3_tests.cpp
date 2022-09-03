// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include <gtest/gtest.h>

#include "vox.geometry/plane3.h"

using namespace vox;

TEST(Plane3, Builder) {
    Plane3 plane = Plane3::builder().withNormal({1, 0, 0}).withPoint({2, 3, 4}).build();

    EXPECT_EQ(Vector3D(1, 0, 0), plane.normal);
    EXPECT_EQ(Point3D(2, 3, 4), plane.point);
}
