//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>

#include "vox.base/constants.h"
#include "vox.math/bounding_box2.h"
#include "vox.math/ray2.h"
#include "vox.math/transform2.h"

namespace vox {

//! Struct that represents ray-surface intersection point.
struct SurfaceRayIntersection2 {
    bool isIntersecting = false;
    double distance = kMaxD;
    Point2D point;
    Vector2D normal;
};

//! Abstract base class for 2-D surface.
class Surface2 {
public:
    //! Local-to-world transform.
    Transform2D transform;

    //! Flips normal.
    bool isNormalFlipped = false;

    //! Constructs a surface with normal direction.
    explicit Surface2(Transform2D transform = Transform2D(), bool isNormalFlipped = false);

    //! Copy constructor.
    Surface2(const Surface2 &other);

    //! Default destructor.
    virtual ~Surface2();

    //! Returns the closest point from the given point \p otherPoint to the
    //! surface.
    [[nodiscard]] Point2D closestPoint(const Point2D &otherPoint) const;

    //! Returns the bounding box of this surface object.
    [[nodiscard]] BoundingBox2D boundingBox() const;

    //! Returns true if the given \p ray intersects with this surface object.
    [[nodiscard]] bool intersects(const Ray2D &ray) const;

    //! Returns the closest distance from the given point \p otherPoint to the
    //! point on the surface.
    [[nodiscard]] double closestDistance(const Point2D &otherPoint) const;

    //! Returns the closest intersection point for given \p ray.
    [[nodiscard]] SurfaceRayIntersection2 closestIntersection(const Ray2D &ray) const;

    //! Returns the normal to the closest point on the surface from the given
    //! point \p otherPoint.
    [[nodiscard]] Vector2D closestNormal(const Point2D &otherPoint) const;

    //! Updates internal spatial query engine.
    virtual void updateQueryEngine();

    //! Returns true if bounding box can be defined.
    [[nodiscard]] virtual bool isBounded() const;

    //! Returns true if the surface is a valid geometry.
    [[nodiscard]] virtual bool isValidGeometry() const;

    //! Returns true if \p otherPoint is inside the volume defined by the
    //! surface.
    [[nodiscard]] bool isInside(const Point2D &otherPoint) const;

protected:
    //! Returns the closest point from the given point \p otherPoint to the
    //! surface in local frame.
    [[nodiscard]] virtual Point2D closestPointLocal(const Point2D &otherPoint) const = 0;

    //! Returns the bounding box of this surface object in local frame.
    [[nodiscard]] virtual BoundingBox2D boundingBoxLocal() const = 0;

    //! Returns the closest intersection point for given \p ray in local frame.
    [[nodiscard]] virtual SurfaceRayIntersection2 closestIntersectionLocal(const Ray2D &ray) const = 0;

    //! Returns the normal to the closest point on the surface from the given
    //! point \p otherPoint in local frame.
    [[nodiscard]] virtual Vector2D closestNormalLocal(const Point2D &otherPoint) const = 0;

    //! Returns true if the given \p ray intersects with this surface object
    //! in local frame.
    [[nodiscard]] virtual bool intersectsLocal(const Ray2D &ray) const;

    //! Returns the closest distance from the given point \p otherPoint to the
    //! point on the surface in local frame.
    [[nodiscard]] virtual double closestDistanceLocal(const Point2D &otherPoint) const;

    //! Returns true if \p otherPoint is inside by given \p depth the volume
    //! defined by the surface in local frame.
    [[nodiscard]] virtual bool isInsideLocal(const Point2D &otherPoint) const;
};

//! Shared pointer for the Surface2 type.
typedef std::shared_ptr<Surface2> Surface2Ptr;

//!
//! \brief Base class for 2-D surface builder.
//!
template <typename DerivedBuilder>
class SurfaceBuilderBase2 {
public:
    //! Returns builder with flipped normal flag.
    DerivedBuilder &withIsNormalFlipped(bool isNormalFlipped);

    //! Returns builder with translation.
    DerivedBuilder &withTranslation(const Vector2D &translation);

    //! Returns builder with orientation.
    DerivedBuilder &withOrientation(double orientation);

    //! Returns builder with transform.
    DerivedBuilder &withTransform(const Transform2D &transform);

protected:
    bool _isNormalFlipped = false;
    Transform2D _transform;
};

template <typename T>
T &SurfaceBuilderBase2<T>::withIsNormalFlipped(bool isNormalFlipped) {
    _isNormalFlipped = isNormalFlipped;
    return static_cast<T &>(*this);
}

template <typename T>
T &SurfaceBuilderBase2<T>::withTranslation(const Vector2D &translation) {
    _transform.setTranslation(translation);
    return static_cast<T &>(*this);
}

template <typename T>
T &SurfaceBuilderBase2<T>::withOrientation(double orientation) {
    _transform.setOrientation(orientation);
    return static_cast<T &>(*this);
}

template <typename T>
T &SurfaceBuilderBase2<T>::withTransform(const Transform2D &transform) {
    _transform = transform;
    return static_cast<T &>(*this);
}

}  // namespace vox
