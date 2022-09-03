//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>

#include "vox.geometry/bvh3.h"
#include "vox.geometry/implicit_surface3.h"

namespace vox {

//!
//! \brief 3-D implicit surface set.
//!
//! This class represents 3-D implicit surface set which extends
//! ImplicitSurface3 by overriding implicit surface-related quries. This is
//! class can hold a collection of other implicit surface instances.
//!
class ImplicitSurfaceSet3 final : public ImplicitSurface3 {
public:
    class Builder;

    //! Constructs an empty implicit surface set.
    ImplicitSurfaceSet3();

    //! Constructs an implicit surface set using list of other surfaces.
    explicit ImplicitSurfaceSet3(std::vector<ImplicitSurface3Ptr> surfaces,
                                 const Transform3D &transform = Transform3D(),
                                 bool isNormalFlipped = false);

    //! Constructs an implicit surface set using list of other surfaces.
    explicit ImplicitSurfaceSet3(const std::vector<Surface3Ptr> &surfaces,
                                 const Transform3D &transform = Transform3D(),
                                 bool isNormalFlipped = false);

    //! Copy constructor.
    ImplicitSurfaceSet3(const ImplicitSurfaceSet3 &other);

    //! Updates internal spatial query engine.
    void updateQueryEngine() override;

    //! Returns true if bounding box can be defined.
    bool isBounded() const override;

    //! Returns true if the surface is a valid geometry.
    bool isValidGeometry() const override;

    //! Returns the number of implicit surfaces.
    size_t numberOfSurfaces() const;

    //! Returns the i-th implicit surface.
    const ImplicitSurface3Ptr &surfaceAt(size_t i) const;

    //! Adds an explicit surface instance.
    void addExplicitSurface(const Surface3Ptr &surface);

    //! Adds an implicit surface instance.
    void addSurface(const ImplicitSurface3Ptr &surface);

    //! Returns builder fox ImplicitSurfaceSet3.
    static Builder builder();

private:
    std::vector<ImplicitSurface3Ptr> _surfaces;
    std::vector<ImplicitSurface3Ptr> _unboundedSurfaces;
    mutable Bvh3<ImplicitSurface3Ptr> _bvh;
    mutable bool _bvhInvalidated = true;

    // Surface3 implementations

    Point3D closestPointLocal(const Point3D &otherPoint) const override;

    BoundingBox3D boundingBoxLocal() const override;

    double closestDistanceLocal(const Point3D &otherPoint) const override;

    bool intersectsLocal(const Ray3D &ray) const override;

    Vector3D closestNormalLocal(const Point3D &otherPoint) const override;

    SurfaceRayIntersection3 closestIntersectionLocal(const Ray3D &ray) const override;

    bool isInsideLocal(const Point3D &otherPoint) const override;

    // ImplicitSurface3 implementations

    double signedDistanceLocal(const Point3D &otherPoint) const override;

    void invalidateBvh();

    void buildBvh() const;
};

//! Shared pointer type for the ImplicitSurfaceSet3.
typedef std::shared_ptr<ImplicitSurfaceSet3> ImplicitSurfaceSet3Ptr;

//!
//! \brief Front-end to create ImplicitSurfaceSet3 objects step by step.
//!
class ImplicitSurfaceSet3::Builder final : public SurfaceBuilderBase3<ImplicitSurfaceSet3::Builder> {
public:
    //! Returns builder with surfaces.
    Builder &withSurfaces(const std::vector<ImplicitSurface3Ptr> &surfaces);

    //! Returns builder with explicit surfaces.
    Builder &withExplicitSurfaces(const std::vector<Surface3Ptr> &surfaces);

    //! Builds ImplicitSurfaceSet3.
    [[nodiscard]] ImplicitSurfaceSet3 build() const;

    //! Builds shared pointer of ImplicitSurfaceSet3 instance.
    [[nodiscard]] ImplicitSurfaceSet3Ptr makeShared() const;

private:
    bool _isNormalFlipped = false;
    std::vector<ImplicitSurface3Ptr> _surfaces;
};

}  // namespace vox