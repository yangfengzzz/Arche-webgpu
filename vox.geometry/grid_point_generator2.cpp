// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/grid_point_generator2.h"

namespace vox {

void GridPointGenerator2::forEachPoint(const BoundingBox2D& boundingBox,
                                       double spacing,
                                       const std::function<bool(const Point2D&)>& callback) const {
    Point2D position;
    double boxWidth = boundingBox.width();
    double boxHeight = boundingBox.height();

    bool shouldQuit = false;
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

}  // namespace vox
