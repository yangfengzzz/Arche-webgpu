//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.geometry/surface3.h"
#include "vox.math/bounding_box3.h"

namespace vox {

//!
//! \brief 3-D sphere geometry.
//!
//! This class represents 3-D sphere geometry which extends Surface3 by
//! overriding surface-related queries.
//!
class Sphere3 final : public Surface3 {
public:
    class Builder;

    //! Center of the sphere.
    Point3D center;

    //! Radius of the sphere.
    double radius = 1.0;

    //! Constructs a sphere with center at (0, 0, 0) and radius of 1.
    explicit Sphere3(const Transform3D& transform = Transform3D(), bool isNormalFlipped = false);

    //! Constructs a sphere with \p center and \p radius.
    Sphere3(const Point3D& center,
            double radius,
            const Transform3D& transform = Transform3D(),
            bool isNormalFlipped = false);

    //! Copy constructor.
    Sphere3(const Sphere3& other);

    //! Returns builder fox Sphere3.
    static Builder builder();

private:
    [[nodiscard]] Point3D closestPointLocal(const Point3D& otherPoint) const override;

    [[nodiscard]] double closestDistanceLocal(const Point3D& otherPoint) const override;

    [[nodiscard]] bool intersectsLocal(const Ray3D& ray) const override;

    [[nodiscard]] BoundingBox3D boundingBoxLocal() const override;

    [[nodiscard]] Vector3D closestNormalLocal(const Point3D& otherPoint) const override;

    [[nodiscard]] SurfaceRayIntersection3 closestIntersectionLocal(const Ray3D& ray) const override;
};

//! Shared pointer for the Sphere3 type.
typedef std::shared_ptr<Sphere3> Sphere3Ptr;

//!
//! \brief Front-end to create Sphere3 objects step by step.
//!
class Sphere3::Builder final : public SurfaceBuilderBase3<Sphere3::Builder> {
public:
    //! Returns builder with sphere center.
    Builder& withCenter(const Point3D& center);

    //! Returns builder with sphere radius.
    Builder& withRadius(double radius);

    //! Builds Sphere3.
    [[nodiscard]] Sphere3 build() const;

    //! Builds shared pointer of Sphere3 instance.
    [[nodiscard]] Sphere3Ptr makeShared() const;

private:
    Point3D _center{0, 0, 0};
    double _radius = 0.0;
};

}  // namespace vox