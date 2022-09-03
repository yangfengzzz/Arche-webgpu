//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.geometry/implicit_surface3.h"
#include "vox.geometry/scalar_field3.h"

namespace vox {

//! Custom 3-D implicit surface using arbitrary function.
class CustomImplicitSurface3 final : public ImplicitSurface3 {
public:
    class Builder;

    //!
    //! Constructs an implicit surface using the given signed-distance function.
    //!
    //! \param func Custom SDF function object.
    //! \param domain Bounding box of the SDF if exists.
    //! \param resolution Finite differencing resolution for derivatives.
    //! \param rayMarchingResolution Ray marching resolution for ray tests.
    //! \param maxNumOfIterations Number of iterations for closest point search.
    //! \param transform Local-to-world transform.
    //! \param isNormalFlipped True if normal is flipped.
    //!
    explicit CustomImplicitSurface3(std::function<double(const Point3D &)> func,
                                    const BoundingBox3D &domain = BoundingBox3D(),
                                    double resolution = 1e-3,
                                    double rayMarchingResolution = 1e-6,
                                    unsigned int maxNumOfIterations = 5,
                                    const Transform3D &transform = Transform3D(),
                                    bool isNormalFlipped = false);

    //! Destructor.
    ~CustomImplicitSurface3() override;

    //! Returns builder for CustomImplicitSurface3.
    static Builder builder();

private:
    std::function<double(const Point3D &)> _func;
    BoundingBox3D _domain;
    double _resolution = 1e-3;
    double _rayMarchingResolution = 1e-6;
    unsigned int _maxNumOfIterations = 5;

    [[nodiscard]] Point3D closestPointLocal(const Point3D &otherPoint) const override;

    [[nodiscard]] bool intersectsLocal(const Ray3D &ray) const override;

    [[nodiscard]] BoundingBox3D boundingBoxLocal() const override;

    [[nodiscard]] Vector3D closestNormalLocal(const Point3D &otherPoint) const override;

    [[nodiscard]] double signedDistanceLocal(const Point3D &otherPoint) const override;

    [[nodiscard]] SurfaceRayIntersection3 closestIntersectionLocal(const Ray3D &ray) const override;

    [[nodiscard]] Vector3D gradientLocal(const Point3D &x) const;
};

//! Shared pointer type for the CustomImplicitSurface3.
typedef std::shared_ptr<CustomImplicitSurface3> CustomImplicitSurface3Ptr;

//!
//! \brief Front-end to create CustomImplicitSurface3 objects step by step.
//!
class CustomImplicitSurface3::Builder final : public SurfaceBuilderBase3<CustomImplicitSurface3::Builder> {
public:
    //! Returns builder with custom signed-distance function
    Builder &withSignedDistanceFunction(const std::function<double(const Point3D &)> &func);

    //! Returns builder with domain.
    Builder &withDomain(const BoundingBox3D &domain);

    //! Returns builder with finite differencing resolution.
    Builder &withResolution(double resolution);

    //! Returns builder with ray marching resolution which determines the ray
    //! intersection quality.
    Builder &withRayMarchingResolution(double rayMarchingResolution);

    //! Returns builder with number of iterations for closest point/normal
    //! searches.
    Builder &withMaxNumberOfIterations(unsigned int numIter);

    //! Builds CustomImplicitSurface3.
    [[nodiscard]] CustomImplicitSurface3 build() const;

    //! Builds shared pointer of CustomImplicitSurface3 instance.
    [[nodiscard]] CustomImplicitSurface3Ptr makeShared() const;

private:
    std::function<double(const Point3D &)> _func;
    BoundingBox3D _domain;
    double _resolution = 1e-3;
    double _rayMarchingResolution = 1e-6;
    unsigned int _maxNumOfIterations = 5;
};

}  // namespace vox