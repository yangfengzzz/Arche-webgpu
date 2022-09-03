//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>

#include "vox.geometry/implicit_surface2.h"

namespace vox {

//!
//! \brief 2-D implicit surface wrapper for generic Surface2 instance.
//!
//! This class represents 2-D implicit surface that converts Surface2 instance
//! to an ImplicitSurface2 object. The conversion is made by evaluating closest
//! point and normal from a given point for the given (explicit) surface. Thus,
//! this conversion won't work for every single surfaces. Use this class only
//! for the basic primitives such as Sphere2 or Box2.
//!
class SurfaceToImplicit2 final : public ImplicitSurface2 {
public:
    class Builder;

    //! Constructs an instance with generic Surface2 instance.
    explicit SurfaceToImplicit2(Surface2Ptr surface,
                                const Transform2D &transform = Transform2D(),
                                bool isNormalFlipped = false);

    //! Copy constructor.
    SurfaceToImplicit2(const SurfaceToImplicit2 &other);

    //! Updates internal spatial query engine.
    void updateQueryEngine() override;

    //! Returns true if bounding box can be defined.
    [[nodiscard]] bool isBounded() const override;

    //! Returns true if the surface is a valid geometry.
    [[nodiscard]] bool isValidGeometry() const override;

    //! Returns the raw surface instance.
    [[nodiscard]] Surface2Ptr surface() const;

    //! Returns builder fox SurfaceToImplicit2.
    static Builder builder();

protected:
    [[nodiscard]] Point2D closestPointLocal(const Point2D &otherPoint) const override;

    [[nodiscard]] double closestDistanceLocal(const Point2D &otherPoint) const override;

    [[nodiscard]] bool intersectsLocal(const Ray2D &ray) const override;

    [[nodiscard]] BoundingBox2D boundingBoxLocal() const override;

    [[nodiscard]] Vector2D closestNormalLocal(const Point2D &otherPoint) const override;

    [[nodiscard]] double signedDistanceLocal(const Point2D &otherPoint) const override;

    [[nodiscard]] SurfaceRayIntersection2 closestIntersectionLocal(const Ray2D &ray) const override;

    [[nodiscard]] bool isInsideLocal(const Point2D &otherPoint) const override;

private:
    Surface2Ptr _surface;
};

//! Shared pointer for the SurfaceToImplicit2 type.
typedef std::shared_ptr<SurfaceToImplicit2> SurfaceToImplicit2Ptr;

//!
//! \brief Front-end to create SurfaceToImplicit2 objects step by step.
//!
class SurfaceToImplicit2::Builder final : public SurfaceBuilderBase2<SurfaceToImplicit2::Builder> {
public:
    //! Returns builder with surface.
    Builder &withSurface(const Surface2Ptr &surface);

    //! Builds SurfaceToImplicit2.
    [[nodiscard]] SurfaceToImplicit2 build() const;

    //! Builds shared pointer of SurfaceToImplicit2 instance.
    [[nodiscard]] SurfaceToImplicit2Ptr makeShared() const;

private:
    Surface2Ptr _surface;
};

}  // namespace vox