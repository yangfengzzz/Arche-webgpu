// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include <gtest/gtest.h>

#include "vox.geometry/array1.h"
#include "vox.geometry/point_simple_list_searcher2.h"

using namespace vox;

TEST(PointSimpleListSearcher2, ForEachNearbyPoint) {
    Array1<Point2D> points = {Point2D(1, 3), Point2D(2, 5), Point2D(-1, 3)};

    PointSimpleListSearcher2 searcher;
    searcher.build(points.accessor());

    searcher.forEachNearbyPoint(Point2D(0, 0), std::sqrt(10.0), [&](size_t i, const Point2D& pt) {
        EXPECT_TRUE(i == 0 || i == 2);

        if (i == 0) {
            EXPECT_EQ(points[0], pt);
        } else if (i == 2) {
            EXPECT_EQ(points[2], pt);
        }
    });
}
