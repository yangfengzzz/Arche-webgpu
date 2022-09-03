// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include <gtest/gtest.h>

#include <vector>

#include "vox.geometry/array1.h"
#include "vox.geometry/array3.h"
#include "vox.geometry/bcc_lattice_point_generator.h"
#include "vox.geometry/point_hash_grid_searcher3.h"
#include "vox.geometry/point_parallel_hash_grid_searcher3.h"
#include "vox.math/bounding_box3.h"

using namespace vox;

TEST(PointHashGridSearcher3, ForEachNearbyPoint) {
    Array1<Point3D> points = {Point3D(0, 1, 3), Point3D(2, 5, 4), Point3D(-1, 3, 0)};

    PointHashGridSearcher3 searcher(4, 4, 4, 2.0 * std::sqrt(10));
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

TEST(PointHashGridSearcher3, ForEachNearbyPointEmpty) {
    Array1<Point3D> points;

    PointHashGridSearcher3 searcher(4, 4, 4, 2.0 * std::sqrt(10));
    searcher.build(points.accessor());

    searcher.forEachNearbyPoint(Point3D(0, 0, 0), std::sqrt(10.0), [](size_t, const Point3D&) {});
}

TEST(PointParallelHashGridSearcher3, Build) {
    Array1<Point3D> points;
    BccLatticePointGenerator pointsGenerator;
    BoundingBox3D bbox(Point3D(0, 0, 0), Point3D(1, 1, 1));
    double spacing = 0.1;

    pointsGenerator.generate(bbox, spacing, &points);

    PointHashGridSearcher3 pointSearcher(4, 4, 4, 0.18);
    pointSearcher.build(points.constAccessor());

    Array3<size_t> grid(4, 4, 4);

    grid.forEachIndex([&](size_t i, size_t j, size_t k) {
        size_t key = pointSearcher.getHashKeyFromBucketIndex(
                Point3I(static_cast<ssize_t>(i), static_cast<ssize_t>(j), static_cast<ssize_t>(k)));
        size_t value = pointSearcher.buckets()[key].size();
        grid(i, j, k) = value;
    });

    PointParallelHashGridSearcher3 parallelSearcher(4, 4, 4, 0.18);
    parallelSearcher.build(points.constAccessor());

    grid.forEachIndex([&](size_t i, size_t j, size_t k) {
        size_t key = parallelSearcher.getHashKeyFromBucketIndex(
                Point3I(static_cast<ssize_t>(i), static_cast<ssize_t>(j), static_cast<ssize_t>(k)));
        size_t start = parallelSearcher.startIndexTable()[key];
        size_t end = parallelSearcher.endIndexTable()[key];
        size_t value = end - start;
        EXPECT_EQ(grid(i, j, k), value);
    });
}

TEST(PointHashGridSearcher3, CopyConstructor) {
    Array1<Point3D> points = {Point3D(0, 1, 3), Point3D(2, 5, 4), Point3D(-1, 3, 0)};

    PointHashGridSearcher3 searcher(4, 4, 4, 2.0 * std::sqrt(10));
    searcher.build(points.accessor());

    PointHashGridSearcher3 searcher2(searcher);
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
