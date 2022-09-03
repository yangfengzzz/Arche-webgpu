// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/grid_point_generator3.h"

namespace vox {

void GridPointGenerator3::forEachPoint(const BoundingBox3D& boundingBox,
                                       double spacing,
                                       const std::function<bool(const Point3D&)>& callback) const {
    Point3D position;
    double boxWidth = boundingBox.width();
    double boxHeight = boundingBox.height();
    double boxDepth = boundingBox.depth();

    bool shouldQuit = false;
    for (int k = 0; k * spacing <= boxDepth && !shouldQuit; ++k) {
        position.z = k * spacing + boundingBox.lower_corner.z;

        for (int j = 0; j * spacing <= boxHeight && !shouldQuit; ++j) {
            position.y = j * spacing + boundingBox.lower_corner.y;

            for (int i = 0; i * spacing <= boxWidth && !shouldQuit; ++i) {
                position.x = i * spacing + boundingBox.lower_corner.x;
                if (!callback(position)) {
                    shouldQuit = true;
                    break;
                }
            }
        }
    }
}

}  // namespace vox
