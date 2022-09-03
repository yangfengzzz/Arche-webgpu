// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/point_generator3.h"

namespace vox {

PointGenerator3::PointGenerator3() = default;

PointGenerator3::~PointGenerator3() = default;

void PointGenerator3::generate(const BoundingBox3D& boundingBox, double spacing, Array1<Point3D>* points) const {
    forEachPoint(boundingBox, spacing, [&points](const Point3D& point) {
        points->append(point);
        return true;
    });
}

}  // namespace vox
