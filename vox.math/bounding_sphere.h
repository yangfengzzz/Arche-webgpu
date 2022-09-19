//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/bounding_box3.h"
#include "vox.math/vector3.h"

namespace vox {
class BoundingSphere {
public:
    /** The center point of the sphere. */
    Point3F center{};
    /** The radius of the sphere. */
    float radius = 0;

    /**
     * Calculate a bounding sphere that fully contains the given points.
     * @param points - The given points
     * @return - The calculated bounding sphere
     */
    static BoundingSphere fromPoints(const std::vector<Point3F>& points);

    /**
     * Calculate a bounding sphere from a given box.
     * @param box - The given box
     * @return - The calculated bounding sphere
     */
    static BoundingSphere fromBox(const BoundingBox3F& box);

    explicit BoundingSphere(const Point3F& center = Point3F(), float radius = 0);
};

}  // namespace vox