//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.geometry/vector_field3.h"

namespace vox {

//! 3-D vector field with custom field function.
class CustomVectorField3 final : public VectorField3 {
public:
    class Builder;

    //!
    //! \brief Constructs a field with given function.
    //!
    //! This constructor creates a field with user-provided function object.
    //! To compute derivatives, such as gradient and Laplacian, finite
    //! differencing is used. Thus, the differencing resolution also can be
    //! provided as the last parameter.
    //!
    explicit CustomVectorField3(std::function<Vector3D(const Point3D &)> customFunction,
                                double derivativeResolution = 1e-3);

    //!
    //! \brief Constructs a field with given field and gradient function.
    //!
    //! This constructor creates a field with user-provided field and gradient
    //! function objects. To compute Laplacian, finite differencing is used.
    //! Thus, the differencing resolution also can be provided as the last
    //! parameter.
    //!
    CustomVectorField3(std::function<Vector3D(const Point3D &)> customFunction,
                       std::function<double(const Point3D &)> customDivergenceFunction,
                       double derivativeResolution = 1e-3);

    //! Constructs a field with given field, gradient, and Laplacian function.
    CustomVectorField3(std::function<Vector3D(const Point3D &)> customFunction,
                       std::function<double(const Point3D &)> customDivergenceFunction,
                       std::function<Vector3D(const Point3D &)> customCurlFunction);

    //! Returns the sampled value at given position \p x.
    [[nodiscard]] Vector3D sample(const Point3D &x) const override;

    //! Returns the divergence at given position \p x.
    [[nodiscard]] double divergence(const Point3D &x) const override;

    //! Returns the curl at given position \p x.
    [[nodiscard]] Vector3D curl(const Point3D &x) const override;

    //! Returns the sampler function.
    [[nodiscard]] std::function<Vector3D(const Point3D &)> sampler() const override;

    //! Returns builder fox CustomVectorField2.
    static Builder builder();

private:
    std::function<Vector3D(const Point3D &)> _customFunction;
    std::function<double(const Point3D &)> _customDivergenceFunction;
    std::function<Vector3D(const Point3D &)> _customCurlFunction;
    double _resolution = 1e-3;
};

//! Shared pointer type for the CustomVectorField3.
typedef std::shared_ptr<CustomVectorField3> CustomVectorField3Ptr;

//!
//! \brief Front-end to create CustomVectorField3 objects step by step.
//!
class CustomVectorField3::Builder final {
public:
    //! Returns builder with field function.
    Builder &withFunction(const std::function<Vector3D(const Point3D &)> &func);

    //! Returns builder with divergence function.
    Builder &withDivergenceFunction(const std::function<double(const Point3D &)> &func);

    //! Returns builder with curl function.
    Builder &withCurlFunction(const std::function<Vector3D(const Point3D &)> &func);

    //! Returns builder with derivative resolution.
    Builder &withDerivativeResolution(double resolution);

    //! Builds CustomVectorField3.
    [[nodiscard]] CustomVectorField3 build() const;

    //! Builds shared pointer of CustomVectorField3 instance.
    [[nodiscard]] CustomVectorField3Ptr makeShared() const;

private:
    double _resolution = 1e-3;
    std::function<Vector3D(const Point3D &)> _customFunction;
    std::function<double(const Point3D &)> _customDivergenceFunction;
    std::function<Vector3D(const Point3D &)> _customCurlFunction;
};

}  // namespace vox