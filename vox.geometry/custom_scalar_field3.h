//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.geometry/scalar_field3.h"

namespace vox {

//! 3-D scalar field with custom field function.
class CustomScalarField3 final : public ScalarField3 {
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
    explicit CustomScalarField3(std::function<double(const Point3D &)> customFunction,
                                double derivativeResolution = 1e-3);

    //!
    //! \brief Constructs a field with given field and gradient function.
    //!
    //! This constructor creates a field with user-provided field and gradient
    //! function objects. To compute Laplacian, finite differencing is used.
    //! Thus, the differencing resolution also can be provided as the last
    //! parameter.
    //!
    CustomScalarField3(std::function<double(const Point3D &)> customFunction,
                       std::function<Vector3D(const Point3D &)> customGradientFunction,
                       double derivativeResolution = 1e-3);

    //! Constructs a field with given field, gradient, and Laplacian function.
    CustomScalarField3(std::function<double(const Point3D &)> customFunction,
                       std::function<Vector3D(const Point3D &)> customGradientFunction,
                       std::function<double(const Point3D &)> customLaplacianFunction);

    //! Returns the sampled value at given position \p x.
    [[nodiscard]] double sample(const Point3D &x) const override;

    //! Returns the sampler function.
    [[nodiscard]] std::function<double(const Point3D &)> sampler() const override;

    //! Returns the gradient vector at given position \p x.
    [[nodiscard]] Vector3D gradient(const Point3D &x) const override;

    //! Returns the Laplacian at given position \p x.
    [[nodiscard]] double laplacian(const Point3D &x) const override;

    //! Returns builder fox CustomScalarField3.
    static Builder builder();

private:
    std::function<double(const Point3D &)> _customFunction;
    std::function<Vector3D(const Point3D &)> _customGradientFunction;
    std::function<double(const Point3D &)> _customLaplacianFunction;
    double _resolution = 1e-3;
};

//! Shared pointer type for the CustomScalarField3.
typedef std::shared_ptr<CustomScalarField3> CustomScalarField3Ptr;

//!
//! \brief Front-end to create CustomScalarField3 objects step by step.
//!
class CustomScalarField3::Builder final {
public:
    //! Returns builder with field function.
    Builder &withFunction(const std::function<double(const Point3D &)> &func);

    //! Returns builder with divergence function.
    Builder &withGradientFunction(const std::function<Vector3D(const Point3D &)> &func);

    //! Returns builder with curl function.
    Builder &withLaplacianFunction(const std::function<double(const Point3D &)> &func);

    //! Returns builder with derivative resolution.
    Builder &withDerivativeResolution(double resolution);

    //! Builds CustomScalarField3.
    [[nodiscard]] CustomScalarField3 build() const;

    //! Builds shared pointer of CustomScalarField3 instance.
    [[nodiscard]] CustomScalarField3Ptr makeShared() const;

private:
    double _resolution = 1e-3;
    std::function<double(const Point3D &)> _customFunction;
    std::function<Vector3D(const Point3D &)> _customGradientFunction;
    std::function<double(const Point3D &)> _customLaplacianFunction;
};

}  // namespace vox