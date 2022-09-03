// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/bcc_lattice_point_generator.h"

namespace vox {

void BccLatticePointGenerator::forEachPoint(const BoundingBox3D& boundingBox,
                                            double spacing,
                                            const std::function<bool(const Point3D&)>& callback) const {
    double halfSpacing = spacing / 2.0;
    double boxWidth = boundingBox.width();
    double boxHeight = boundingBox.height();
    double boxDepth = boundingBox.depth();

    Point3D position;
    bool hasOffset = false;
    bool shouldQuit = false;
    for (int k = 0; k * halfSpacing <= boxDepth && !shouldQuit; ++k) {
        position.z = k * halfSpacing + boundingBox.lower_corner.z;

        double offset = (hasOffset) ? halfSpacing : 0.0;

        for (int j = 0; j * spacing + offset <= boxHeight && !shouldQuit; ++j) {
            position.y = j * spacing + offset + boundingBox.lower_corner.y;

            for (int i = 0; i * spacing + offset <= boxWidth; ++i) {
                position.x = i * spacing + offset + boundingBox.lower_corner.x;
                if (!callback(position)) {
                    shouldQuit = true;
                    break;
                }
            }
        }

        hasOffset = !hasOffset;
    }
}

}  // namespace vox
