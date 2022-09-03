// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include <gtest/gtest.h>

#include "vox.geometry/array1.h"
#include "vox.geometry/array2.h"
#include "vox.geometry/point_hash_grid_searcher2.h"
#include "vox.geometry/point_parallel_hash_grid_searcher2.h"
#include "vox.geometry/triangle_point_generator.h"
#include "vox.math/bounding_box2.h"

using namespace vox;

TEST(PointHashGridSearcher2, ForEachNearbyPoint) {
    Array1<Point2D> points = {Point2D(1, 3), Point2D(2, 5), Point2D(-1, 3)};

    PointHashGridSearcher2 searcher(4, 4, 2.0 * std::sqrt(10));
    searcher.build(points.accessor());

    searcher.forEachNearbyPoint(Point2D(0, 0), std::sqrt(10.0), [&points](size_t i, const Point2D& pt) {
        EXPECT_TRUE(i == 0 || i == 2);

        if (i == 0) {
            EXPECT_EQ(points[0], pt);
        } else if (i == 2) {
            EXPECT_EQ(points[2], pt);
        }
    });
}

TEST(PointHashGridSearcher2, ForEachNearbyPointEmpty) {
    Array1<Point2D> points;

    PointHashGridSearcher2 searcher(4, 4, 2.0 * std::sqrt(10));
    searcher.build(points.accessor());

    searcher.forEachNearbyPoint(Point2D(0, 0), std::sqrt(10.0), [](size_t, const Point2D&) {});
}

TEST(PointParallelHashGridSearcher2, Build) {
    Array1<Point2D> points;
    TrianglePointGenerator pointsGenerator;
    BoundingBox2D bbox(Point2D(0, 0), Point2D(1, 1));
    double spacing = 0.1;

    pointsGenerator.generate(bbox, spacing, &points);

    PointHashGridSearcher2 pointSearcher(4, 4, 0.18);
    pointSearcher.build(points.constAccessor());

    Array2<size_t> grid(4, 4);

    for (size_t j = 0; j < grid.size().y; ++j) {
        for (size_t i = 0; i < grid.size().x; ++i) {
            size_t key =
                    pointSearcher.getHashKeyFromBucketIndex(Point2I(static_cast<ssize_t>(i), static_cast<ssize_t>(j)));
            size_t value = pointSearcher.buckets()[key].size();
            grid(i, j) = value;
        }
    }

    PointParallelHashGridSearcher2 parallelSearcher(4, 4, 0.18);
    parallelSearcher.build(points.constAccessor());

    for (size_t j = 0; j < grid.size().y; ++j) {
        for (size_t i = 0; i < grid.size().x; ++i) {
            size_t key = parallelSearcher.getHashKeyFromBucketIndex(
                    Point2I(static_cast<ssize_t>(i), static_cast<ssize_t>(j)));
            size_t start = parallelSearcher.startIndexTable()[key];
            size_t end = parallelSearcher.endIndexTable()[key];
            size_t value = end - start;
            EXPECT_EQ(grid(i, j), value);
        }
    }
}
