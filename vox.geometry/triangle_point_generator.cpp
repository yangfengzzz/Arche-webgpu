// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/triangle_point_generator.h"

using namespace vox;

void TrianglePointGenerator::forEachPoint(const BoundingBox2D& boundingBox,
                                          double spacing,
                                          const std::function<bool(const Point2D&)>& callback) const {
    const double halfSpacing = spacing / 2.0;
    const double ySpacing = spacing * std::sqrt(3.0) / 2.0;
    double boxWidth = boundingBox.width();
    double boxHeight = boundingBox.height();

    Point2D position;
    bool hasOffset = false;
    bool shouldQuit = false;
    for (int j = 0; j * ySpacing <= boxHeight && !shouldQuit; ++j) {
        position.y = j * ySpacing + boundingBox.lower_corner.y;

        double offset = (hasOffset) ? halfSpacing : 0.0;

        for (int i = 0; i * spacing + offset <= boxWidth && !shouldQuit; ++i) {
            position.x = i * spacing + offset + boundingBox.lower_corner.x;
            if (!callback(position)) {
                shouldQuit = true;
                break;
            }
        }

        hasOffset = !hasOffset;
    }
}
