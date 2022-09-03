//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.geometry/implicit_surface2.h"
#include "vox.geometry/scalar_field2.h"

namespace vox {

//! Custom 2-D implicit surface using arbitrary function.
class CustomImplicitSurface2 final : public ImplicitSurface2 {
public:
    class Builder;

    //!
    //! Constructs an implicit surface using the given signed-distance function.
    //!
    //! \param func Custom SDF function object.
    //! \param domain Bounding box of the SDF if exists.
    //! \param resolution Finite differencing resolution for derivatives.
    //! \param rayMarchingResolution Ray marching resolution for ray tests.
    //! \param numberOfIterations Number of iterations for closest point search.
    //! \param transform Local-to-world transform.
    //! \param isNormalFlipped True if normal is flipped.
    //!
    explicit CustomImplicitSurface2(std::function<double(const Point2D &)> func,
                                    const BoundingBox2D &domain = BoundingBox2D(),
                                    double resolution = 1e-3,
                                    double rayMarchingResolution = 1e-6,
                                    unsigned int numberOfIterations = 5,
                                    const Transform2D &transform = Transform2D(),
                                    bool isNormalFlipped = false);

    //! Destructor.
    ~CustomImplicitSurface2() override;

    //! Returns builder for CustomImplicitSurface2.
    static Builder builder();

private:
    std::function<double(const Point2D &)> _func;
    BoundingBox2D _domain;
    double _resolution = 1e-3;
    double _rayMarchingResolution = 1e-6;
    unsigned int _maxNumOfIterations = 5;

    [[nodiscard]] Point2D closestPointLocal(const Point2D &otherPoint) const override;

    [[nodiscard]] bool intersectsLocal(const Ray2D &ray) const override;

    [[nodiscard]] BoundingBox2D boundingBoxLocal() const override;

    [[nodiscard]] Vector2D closestNormalLocal(const Point2D &otherPoint) const override;

    [[nodiscard]] double signedDistanceLocal(const Point2D &otherPoint) const override;

    [[nodiscard]] SurfaceRayIntersection2 closestIntersectionLocal(const Ray2D &ray) const override;

    [[nodiscard]] Vector2D gradientLocal(const Point2D &x) const;
};

//! Shared pointer type for the CustomImplicitSurface2.
typedef std::shared_ptr<CustomImplicitSurface2> CustomImplicitSurface2Ptr;

//!
//! \brief Front-end to create CustomImplicitSurface2 objects step by step.
//!
class CustomImplicitSurface2::Builder final : public SurfaceBuilderBase2<CustomImplicitSurface2::Builder> {
public:
    //! Returns builder with custom signed-distance function
    Builder &withSignedDistanceFunction(const std::function<double(const Point2D &)> &func);

    //! Returns builder with domain.
    Builder &withDomain(const BoundingBox2D &domain);

    //! Returns builder with finite differencing resolution.
    Builder &withResolution(double resolution);

    //! Returns builder with ray marching resolution which determines the ray
    //! intersection quality.
    Builder &withRayMarchingResolution(double rayMarchingResolution);

    //! Returns builder with number of iterations for closest point/normal
    //! searches.
    Builder &withMaxNumberOfIterations(unsigned int numIter);

    //! Builds CustomImplicitSurface2.
    [[nodiscard]] CustomImplicitSurface2 build() const;

    //! Builds shared pointer of CustomImplicitSurface2 instance.
    [[nodiscard]] CustomImplicitSurface2Ptr makeShared() const;

private:
    std::function<double(const Point2D &)> _func;
    BoundingBox2D _domain;
    double _resolution = 1e-3;
    double _rayMarchingResolution = 1e-6;
    unsigned int _maxNumOfIterations = 5;
};

}  // namespace vox