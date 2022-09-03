// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include <gtest/gtest.h>

#include <vector>

#include "vox.geometry/array1.h"
#include "vox.geometry/point_simple_list_searcher3.h"

using namespace vox;

TEST(PointSimpleListSearcher3, ForEachNearbyPoint) {
    Array1<Point3D> points = {Point3D(0, 1, 3), Point3D(2, 5, 4), Point3D(-1, 3, 0)};

    PointSimpleListSearcher3 searcher;
    searcher.build(points.accessor());

    int cnt = 0;
    searcher.forEachNearbyPoint(Point3D(0, 0, 0), std::sqrt(10.0), [&](size_t i, const Point3D& pt) {
        EXPECT_TRUE(i == 0 || i == 2);

        if (i == 0) {
            EXPECT_EQ(points[0], pt);
        } else if (i == 2) {
            EXPECT_EQ(points[2], pt);
        }

        ++cnt;
    });

    EXPECT_EQ(2, cnt);
}

TEST(PointSimpleListSearcher3, CopyConstructor) {
    Array1<Point3D> points = {Point3D(0, 1, 3), Point3D(2, 5, 4), Point3D(-1, 3, 0)};

    PointSimpleListSearcher3 searcher;
    searcher.build(points.accessor());

    PointSimpleListSearcher3 searcher2(searcher);
    int cnt = 0;
    searcher2.forEachNearbyPoint(Point3D(0, 0, 0), std::sqrt(10.0), [&](size_t i, const Point3D& pt) {
        EXPECT_TRUE(i == 0 || i == 2);

        if (i == 0) {
            EXPECT_EQ(points[0], pt);
        } else if (i == 2) {
            EXPECT_EQ(points[2], pt);
        }

        ++cnt;
    });

    EXPECT_EQ(2, cnt);
}
