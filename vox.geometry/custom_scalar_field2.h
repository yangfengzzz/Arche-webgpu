//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.geometry/scalar_field2.h"

namespace vox {

//! 2-D scalar field with custom field function.
class CustomScalarField2 final : public ScalarField2 {
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
    explicit CustomScalarField2(std::function<double(const Point2D &)> customFunction,
                                double derivativeResolution = 1e-3);

    //!
    //! \brief Constructs a field with given field and gradient function.
    //!
    //! This constructor creates a field with user-provided field and gradient
    //! function objects. To compute Laplacian, finite differencing is used.
    //! Thus, the differencing resolution also can be provided as the last
    //! parameter.
    //!
    CustomScalarField2(std::function<double(const Point2D &)> customFunction,
                       std::function<Vector2D(const Point2D &)> customGradientFunction,
                       double derivativeResolution = 1e-3);

    //! Constructs a field with given field, gradient, and Laplacian function.
    CustomScalarField2(std::function<double(const Point2D &)> customFunction,
                       std::function<Vector2D(const Point2D &)> customGradientFunction,
                       std::function<double(const Point2D &)> customLaplacianFunction);

    //! Returns the sampled value at given position \p x.
    [[nodiscard]] double sample(const Point2D &x) const override;

    //! Returns the sampler function.
    [[nodiscard]] std::function<double(const Point2D &)> sampler() const override;

    //! Returns the gradient vector at given position \p x.
    [[nodiscard]] Vector2D gradient(const Point2D &x) const override;

    //! Returns the Laplacian at given position \p x.
    [[nodiscard]] double laplacian(const Point2D &x) const override;

    //! Returns builder fox CustomScalarField2.
    static Builder builder();

private:
    std::function<double(const Point2D &)> _customFunction;
    std::function<Vector2D(const Point2D &)> _customGradientFunction;
    std::function<double(const Point2D &)> _customLaplacianFunction;
    double _resolution = 1e-3;
};

//! Shared pointer type for the CustomScalarField2.
typedef std::shared_ptr<CustomScalarField2> CustomScalarField2Ptr;

//!
//! \brief Front-end to create CustomScalarField2 objects step by step.
//!
class CustomScalarField2::Builder final {
public:
    //! Returns builder with field function.
    Builder &withFunction(const std::function<double(const Point2D &)> &func);

    //! Returns builder with divergence function.
    Builder &withGradientFunction(const std::function<Vector2D(const Point2D &)> &func);

    //! Returns builder with curl function.
    Builder &withLaplacianFunction(const std::function<double(const Point2D &)> &func);

    //! Returns builder with derivative resolution.
    Builder &withDerivativeResolution(double resolution);

    //! Builds CustomScalarField2.
    [[nodiscard]] CustomScalarField2 build() const;

    //! Builds shared pointer of CustomScalarField2 instance.
    [[nodiscard]] CustomScalarField2Ptr makeShared() const;

private:
    double _resolution = 1e-3;
    std::function<double(const Point2D &)> _customFunction;
    std::function<Vector2D(const Point2D &)> _customGradientFunction;
    std::function<double(const Point2D &)> _customLaplacianFunction;
};

}  // namespace vox