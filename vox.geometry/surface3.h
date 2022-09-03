//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>

#include "vox.base/constants.h"
#include "vox.math/bounding_box3.h"
#include "vox.math/ray3.h"
#include "vox.math/transform3.h"

namespace vox {

//! Struct that represents ray-surface intersection point.
struct SurfaceRayIntersection3 {
    bool isIntersecting = false;
    double distance = kMaxD;
    Point3D point;
    Vector3D normal;
};

//! Abstract base class for 3-D surface.
class Surface3 {
public:
    //! Local-to-world transform.
    Transform3D transform;

    //! Flips normal when calling Surface3::closestNormal(...).
    bool isNormalFlipped = false;

    //! Constructs a surface with normal direction.
    explicit Surface3(Transform3D transform = Transform3D(), bool isNormalFlipped = false);

    //! Copy constructor.
    Surface3(const Surface3 &other);

    //! Default destructor.
    virtual ~Surface3();

    //! Returns the closest point from the given point \p otherPoint to the
    //! surface.
    [[nodiscard]] Point3D closestPoint(const Point3D &otherPoint) const;

    //! Returns the bounding box of this surface object.
    [[nodiscard]] BoundingBox3D boundingBox() const;

    //! Returns true if the given \p ray intersects with this surface object.
    [[nodiscard]] bool intersects(const Ray3D &ray) const;

    //! Returns the closest distance from the given point \p otherPoint to the
    //! point on the surface.
    [[nodiscard]] double closestDistance(const Point3D &otherPoint) const;

    //! Returns the closest intersection point for given \p ray.
    [[nodiscard]] SurfaceRayIntersection3 closestIntersection(const Ray3D &ray) const;

    //! Returns the normal to the closest point on the surface from the given
    //! point \p otherPoint.
    [[nodiscard]] Vector3D closestNormal(const Point3D &otherPoint) const;

    //! Updates internal spatial query engine.
    virtual void updateQueryEngine();

    //! Returns true if bounding box can be defined.
    [[nodiscard]] virtual bool isBounded() const;

    //! Returns true if the surface is a valid geometry.
    [[nodiscard]] virtual bool isValidGeometry() const;

    //! Returns true if \p otherPoint is inside the volume defined by the
    //! surface.
    [[nodiscard]] bool isInside(const Point3D &otherPoint) const;

protected:
    //! Returns the closest point from the given point \p otherPoint to the
    //! surface in local frame.
    [[nodiscard]] virtual Point3D closestPointLocal(const Point3D &otherPoint) const = 0;

    //! Returns the bounding box of this surface object in local frame.
    [[nodiscard]] virtual BoundingBox3D boundingBoxLocal() const = 0;

    //! Returns the closest intersection point for given \p ray in local frame.
    [[nodiscard]] virtual SurfaceRayIntersection3 closestIntersectionLocal(const Ray3D &ray) const = 0;

    //! Returns the normal to the closest point on the surface from the given
    //! point \p otherPoint in local frame.
    [[nodiscard]] virtual Vector3D closestNormalLocal(const Point3D &otherPoint) const = 0;

    //! Returns true if the given \p ray intersects with this surface object
    //! in local frame.
    [[nodiscard]] virtual bool intersectsLocal(const Ray3D &ray) const;

    //! Returns the closest distance from the given point \p otherPoint to the
    //! point on the surface in local frame.
    [[nodiscard]] virtual double closestDistanceLocal(const Point3D &otherPoint) const;

    //! Returns true if \p otherPoint is inside by given \p depth the volume
    //! defined by the surface in local frame.
    [[nodiscard]] virtual bool isInsideLocal(const Point3D &otherPoint) const;
};

//! Shared pointer for the Surface3 type.
typedef std::shared_ptr<Surface3> Surface3Ptr;

//!
//! \brief Base class for 3-D surface builder.
//!
template <typename DerivedBuilder>
class SurfaceBuilderBase3 {
public:
    //! Returns builder with flipped normal flag.
    DerivedBuilder &withIsNormalFlipped(bool isNormalFlipped);

    //! Returns builder with translation.
    DerivedBuilder &withTranslation(const Vector3D &translation);

    //! Returns builder with orientation.
    DerivedBuilder &withOrientation(const QuaternionD &orientation);

    //! Returns builder with transform.
    DerivedBuilder &withTransform(const Transform3D &transform);

protected:
    bool _isNormalFlipped = false;
    Transform3D _transform;
};

template <typename T>
T &SurfaceBuilderBase3<T>::withIsNormalFlipped(bool isNormalFlipped) {
    _isNormalFlipped = isNormalFlipped;
    return static_cast<T &>(*this);
}

template <typename T>
T &SurfaceBuilderBase3<T>::withTranslation(const Vector3D &translation) {
    _transform.setTranslation(translation);
    return static_cast<T &>(*this);
}

template <typename T>
T &SurfaceBuilderBase3<T>::withOrientation(const QuaternionD &orientation) {
    _transform.setOrientation(orientation);
    return static_cast<T &>(*this);
}

template <typename T>
T &SurfaceBuilderBase3<T>::withTransform(const Transform3D &transform) {
    _transform = transform;
    return static_cast<T &>(*this);
}

}  // namespace vox
