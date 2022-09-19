//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cassert>
#include <cmath>
#include <optional>

#include "vox.math/bounding_box3.h"
#include "vox.math/bounding_plane3.h"

namespace vox {

/**
 * Frustum face
 */
enum FrustumFace : uint32_t {
    /** Near face */
    Near,
    /** Far face */
    Far,
    /** Left face */
    Left,
    /** Right face */
    Right,
    /** Bottom face */
    Bottom,
    /** Top face */
    Top
};

struct BoundingFrustum {
    /** The near plane of this frustum. */
    BoundingPlane3F near;
    /** The far plane of this frustum. */
    BoundingPlane3F far;
    /** The left plane of this frustum. */
    BoundingPlane3F left;
    /** The right plane of this frustum. */
    BoundingPlane3F right;
    /** The bottom plane of this frustum. */
    BoundingPlane3F bottom;
    /** The top plane of this frustum. */
    BoundingPlane3F top;

    BoundingFrustum();

    /**
     * Constructor of BoundingFrustum.
     * @param matrix - The view-projection matrix
     */
    explicit BoundingFrustum(const Matrix4x4F &matrix);

    //! Copy constructor.
    BoundingFrustum(const BoundingFrustum &other) = default;

    /**
   * Get the plane by the given frustum face.
   * @param face - The frustum face
   * @returns The plane get
     */
    [[nodiscard]] BoundingPlane3F getPlane(FrustumFace face) const;

    /**
     * update all planes from the given matrix.
     * @param matrix - The given view-projection matrix
     */
    void calculateFromMatrix(const Matrix4x4F &matrix);

    /**
     * Get whether or not a specified bounding box intersects with this frustum (Contains or Intersects).
     * @param box - The box for testing
     * @returns True if bounding box intersects with this frustum, false otherwise
     */
    [[nodiscard]] bool intersectsBox(const BoundingBox3F &box) const;
};

}  // namespace vox
