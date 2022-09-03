//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.geometry/surface2.h"

namespace vox {

//!
//! \brief 2-D plane geometry.
//!
//! This class represents 2-D plane geometry which extends Surface2 by
//! overriding surface-related queries.
//!
class Plane2 final : public Surface2 {
public:
    class Builder;

    //! Plane normal.
    Vector2D normal = Vector2D(0, 1);

    //! Point that lies on the plane.
    Point2D point;

    //! Constructs a plane that crosses (0, 0) with surface normal (0, 1).
    explicit Plane2(const Transform2D& transform = Transform2D(), bool isNormalFlipped = false);

    //! Constructs a plane that cross \p point with surface normal \p normal.
    Plane2(const Vector2D& normal,
           const Point2D& point,
           const Transform2D& transform = Transform2D(),
           bool isNormalFlipped = false);

    //! Copy constructor.
    Plane2(const Plane2& other);

    //! Returns true if bounding box can be defined.
    [[nodiscard]] bool isBounded() const override;

    //! Returns builder fox Plane2.
    static Builder builder();

private:
    [[nodiscard]] Point2D closestPointLocal(const Point2D& otherPoint) const override;

    [[nodiscard]] bool intersectsLocal(const Ray2D& ray) const override;

    [[nodiscard]] BoundingBox2D boundingBoxLocal() const override;

    [[nodiscard]] Vector2D closestNormalLocal(const Point2D& otherPoint) const override;

    [[nodiscard]] SurfaceRayIntersection2 closestIntersectionLocal(const Ray2D& ray) const override;
};

//! Shared pointer for the Plane2 type.
typedef std::shared_ptr<Plane2> Plane2Ptr;

//!
//! \brief Front-end to create Plane2 objects step by step.
//!
class Plane2::Builder final : public SurfaceBuilderBase2<Plane2::Builder> {
public:
    //! Returns builder with plane normal.
    Builder& withNormal(const Vector2D& normal);

    //! Returns builder with point on the plane.
    Builder& withPoint(const Point2D& point);

    //! Builds Plane2.
    [[nodiscard]] Plane2 build() const;

    //! Builds shared pointer of Plane2 instance.
    [[nodiscard]] Plane2Ptr makeShared() const;

private:
    Vector2D _normal{0, 1};
    Point2D _point{0, 0};
};

}  // namespace vox