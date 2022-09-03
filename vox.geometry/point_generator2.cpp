// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/point_generator2.h"

namespace vox {

PointGenerator2::PointGenerator2() = default;

PointGenerator2::~PointGenerator2() = default;

void PointGenerator2::generate(const BoundingBox2D& boundingBox, double spacing, Array1<Point2D>* points) const {
    forEachPoint(boundingBox, spacing, [&points](const Point2D& point) {
        points->append(point);
        return true;
    });
}

}  // namespace vox
