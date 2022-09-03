//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.geometry/vector_field2.h"

namespace vox {

//! 2-D vector field with custom field function.
class CustomVectorField2 final : public VectorField2 {
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
    explicit CustomVectorField2(std::function<Vector2D(const Point2D &)> customFunction,
                                double derivativeResolution = 1e-3);

    //!
    //! \brief Constructs a field with given field and gradient function.
    //!
    //! This constructor creates a field with user-provided field and gradient
    //! function objects. To compute Laplacian, finite differencing is used.
    //! Thus, the differencing resolution also can be provided as the last
    //! parameter.
    //!
    CustomVectorField2(std::function<Vector2D(const Point2D &)> customFunction,
                       std::function<double(const Point2D &)> customDivergenceFunction,
                       double derivativeResolution = 1e-3);

    //! Constructs a field with given field, gradient, and Laplacian function.
    CustomVectorField2(std::function<Vector2D(const Point2D &)> customFunction,
                       std::function<double(const Point2D &)> customDivergenceFunction,
                       std::function<double(const Point2D &)> customCurlFunction);

    //! Returns the sampled value at given position \p x.
    [[nodiscard]] Vector2D sample(const Point2D &x) const override;

    //! Returns the divergence at given position \p x.
    [[nodiscard]] double divergence(const Point2D &x) const override;

    //! Returns the curl at given position \p x.
    [[nodiscard]] double curl(const Point2D &x) const override;

    //! Returns the sampler function.
    [[nodiscard]] std::function<Vector2D(const Point2D &)> sampler() const override;

    //! Returns builder fox CustomVectorField2.
    static Builder builder();

private:
    std::function<Vector2D(const Point2D &)> _customFunction;
    std::function<double(const Point2D &)> _customDivergenceFunction;
    std::function<double(const Point2D &)> _customCurlFunction;
    double _resolution = 1e-3;
};

//! Shared pointer type for the CustomVectorField2.
typedef std::shared_ptr<CustomVectorField2> CustomVectorField2Ptr;

//!
//! \brief Front-end to create CustomVectorField2 objects step by step.
//!
class CustomVectorField2::Builder final {
public:
    //! Returns builder with field function.
    Builder &withFunction(const std::function<Vector2D(const Point2D &)> &func);

    //! Returns builder with divergence function.
    Builder &withDivergenceFunction(const std::function<double(const Point2D &)> &func);

    //! Returns builder with curl function.
    Builder &withCurlFunction(const std::function<double(const Point2D &)> &func);

    //! Returns builder with derivative resolution.
    Builder &withDerivativeResolution(double resolution);

    //! Builds CustomVectorField2.
    [[nodiscard]] CustomVectorField2 build() const;

    //! Builds shared pointer of CustomVectorField2 instance.
    [[nodiscard]] CustomVectorField2Ptr makeShared() const;

private:
    double _resolution = 1e-3;
    std::function<Vector2D(const Point2D &)> _customFunction;
    std::function<double(const Point2D &)> _customDivergenceFunction;
    std::function<double(const Point2D &)> _customCurlFunction;
};

}  // namespace vox