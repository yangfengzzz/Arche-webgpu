//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.geometry/surface3.h"

namespace vox {

//!
//! \brief 3-D plane geometry.
//!
//! This class represents 3-D plane geometry which extends Surface3 by
//! overriding surface-related queries.
//!
class Plane3 final : public Surface3 {
public:
    class Builder;

    //! Plane normal.
    Vector3D normal = Vector3D(0, 1, 0);

    //! Point that lies on the plane.
    Point3D point;

    //! Constructs a plane that crosses (0, 0, 0) with surface normal (0, 1, 0).
    explicit Plane3(const Transform3D& transform = Transform3D(), bool isNormalFlipped = false);

    //! Constructs a plane that cross \p point with surface normal \p normal.
    Plane3(const Vector3D& normal,
           const Point3D& point,
           const Transform3D& transform = Transform3D(),
           bool isNormalFlipped = false);

    //! Constructs a plane with three points on the surface. The normal will be
    //! set using the counter clockwise direction.
    Plane3(const Point3D& point0,
           const Point3D& point1,
           const Point3D& point2,
           const Transform3D& transform = Transform3D(),
           bool isNormalFlipped = false);

    //! Copy constructor.
    Plane3(const Plane3& other);

    //! Returns true if bounding box can be defined.
    [[nodiscard]] bool isBounded() const override;

    //! Returns builder fox Plane3.
    static Builder builder();

protected:
    [[nodiscard]] Point3D closestPointLocal(const Point3D& otherPoint) const override;

    [[nodiscard]] bool intersectsLocal(const Ray3D& ray) const override;

    [[nodiscard]] BoundingBox3D boundingBoxLocal() const override;

    [[nodiscard]] Vector3D closestNormalLocal(const Point3D& otherPoint) const override;

    [[nodiscard]] SurfaceRayIntersection3 closestIntersectionLocal(const Ray3D& ray) const override;
};

//! Shared pointer for the Plane3 type.
typedef std::shared_ptr<Plane3> Plane3Ptr;

//!
//! \brief Front-end to create Plane3 objects step by step.
//!
class Plane3::Builder final : public SurfaceBuilderBase3<Plane3::Builder> {
public:
    //! Returns builder with plane normal.
    Builder& withNormal(const Vector3D& normal);

    //! Returns builder with point on the plane.
    Builder& withPoint(const Point3D& point);

    //! Builds Plane3.
    [[nodiscard]] Plane3 build() const;

    //! Builds shared pointer of Plane3 instance.
    [[nodiscard]] Plane3Ptr makeShared() const;

private:
    Vector3D _normal{0, 1, 0};
    Point3D _point{0, 0, 0};
};

}  // namespace vox