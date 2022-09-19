//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.math/bounding_sphere.h"

#include <vector>

namespace vox {
BoundingSphere::BoundingSphere(const Point3F& center, float radius) : center(center), radius(radius) {}

BoundingSphere BoundingSphere::fromBox(const BoundingBox3F& box) {
    BoundingSphere sphere;
    sphere.center.x = (box.lower_corner.x + box.upper_corner.x) * 0.5f;
    sphere.center.y = (box.lower_corner.y + box.upper_corner.y) * 0.5f;
    sphere.center.z = (box.lower_corner.z + box.upper_corner.z) * 0.5f;
    sphere.radius = sphere.center.distanceTo(box.upper_corner);
    return sphere;
}

BoundingSphere BoundingSphere::fromPoints(const std::vector<Point3F>& points) {
    BoundingSphere sphere;
    size_t len = points.size();

    Point3F center;
    // Calculate the center of the sphere.
    for (size_t i = 0; i < len; ++i) {
        center.x += points[i].x;
        center.y += points[i].y;
        center.z += points[i].z;
    }

    // The center of the sphere.
    sphere.center = center / float(len);

    // Calculate the radius of the sphere.
    float radius = 0.0;
    for (size_t i = 0; i < len; ++i) {
        float distance = center.distanceSquaredTo(points[i]);
        if (distance > radius) {
            radius = distance;
        }
    }
    // The radius of the sphere.
    sphere.radius = std::sqrt(radius);
    return sphere;
}

}  // namespace vox