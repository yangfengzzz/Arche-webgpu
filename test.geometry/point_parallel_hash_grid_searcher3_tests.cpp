// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include <gtest/gtest.h>

#include <vector>

#include "vox.geometry/array1.h"
#include "vox.geometry/point_parallel_hash_grid_searcher3.h"

using namespace vox;

TEST(PointParallelHashGridSearcher3, ForEachNearbyPoint) {
    Array1<Point3D> points = {Point3D(0, 1, 3), Point3D(2, 5, 4), Point3D(-1, 3, 0)};

    PointParallelHashGridSearcher3 searcher(4, 4, 4, std::sqrt(10));
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

TEST(PointParallelHashGridSearcher3, ForEachNearbyPointEmpty) {
    Array1<Point3D> points;

    PointParallelHashGridSearcher3 searcher(4, 4, 4, std::sqrt(10));
    searcher.build(points.accessor());

    searcher.forEachNearbyPoint(Point3D(0, 0, 0), std::sqrt(10.0), [](size_t, const Point3D&) {});
}

TEST(PointParallelHashGridSearcher3, CopyConstructor) {
    Array1<Point3D> points = {Point3D(0, 1, 3), Point3D(2, 5, 4), Point3D(-1, 3, 0)};

    PointParallelHashGridSearcher3 searcher(4, 4, 4, std::sqrt(10));
    searcher.build(points.accessor());

    PointParallelHashGridSearcher3 searcher2(searcher);
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
