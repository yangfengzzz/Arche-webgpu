// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/custom_scalar_field2.h"

#include <utility>

using namespace vox;

CustomScalarField2::CustomScalarField2(std::function<double(const Point2D &)> customFunction,
                                       double derivativeResolution)
    : _customFunction(std::move(customFunction)), _resolution(derivativeResolution) {}

CustomScalarField2::CustomScalarField2(std::function<double(const Point2D &)> customFunction,
                                       std::function<Vector2D(const Point2D &)> customGradientFunction,
                                       double derivativeResolution)
    : _customFunction(std::move(customFunction)),
      _customGradientFunction(std::move(customGradientFunction)),
      _resolution(derivativeResolution) {}

CustomScalarField2::CustomScalarField2(std::function<double(const Point2D &)> customFunction,
                                       std::function<Vector2D(const Point2D &)> customGradientFunction,
                                       std::function<double(const Point2D &)> customLaplacianFunction)
    : _customFunction(std::move(customFunction)),
      _customGradientFunction(std::move(customGradientFunction)),
      _customLaplacianFunction(std::move(customLaplacianFunction)),
      _resolution(1e-3) {}

double CustomScalarField2::sample(const Point2D &x) const { return _customFunction(x); }

std::function<double(const Point2D &)> CustomScalarField2::sampler() const { return _customFunction; }

Vector2D CustomScalarField2::gradient(const Point2D &x) const {
    if (_customGradientFunction) {
        return _customGradientFunction(x);
    } else {
        double left = _customFunction(x - Vector2D(0.5 * _resolution, 0.0));
        double right = _customFunction(x + Vector2D(0.5 * _resolution, 0.0));
        double bottom = _customFunction(x - Vector2D(0.0, 0.5 * _resolution));
        double top = _customFunction(x + Vector2D(0.0, 0.5 * _resolution));

        return {(right - left) / _resolution, (top - bottom) / _resolution};
    }
}

double CustomScalarField2::laplacian(const Point2D &x) const {
    if (_customLaplacianFunction) {
        return _customLaplacianFunction(x);
    } else {
        double center = _customFunction(x);
        double left = _customFunction(x - Vector2D(0.5 * _resolution, 0.0));
        double right = _customFunction(x + Vector2D(0.5 * _resolution, 0.0));
        double bottom = _customFunction(x - Vector2D(0.0, 0.5 * _resolution));
        double top = _customFunction(x + Vector2D(0.0, 0.5 * _resolution));

        return (left + right + bottom + top - 4.0 * center) / (_resolution * _resolution);
    }
}

CustomScalarField2::Builder CustomScalarField2::builder() { return {}; }

CustomScalarField2::Builder &CustomScalarField2::Builder::withFunction(
        const std::function<double(const Point2D &)> &func) {
    _customFunction = func;
    return *this;
}

CustomScalarField2::Builder &CustomScalarField2::Builder::withGradientFunction(
        const std::function<Vector2D(const Point2D &)> &func) {
    _customGradientFunction = func;
    return *this;
}

CustomScalarField2::Builder &CustomScalarField2::Builder::withLaplacianFunction(
        const std::function<double(const Point2D &)> &func) {
    _customLaplacianFunction = func;
    return *this;
}

CustomScalarField2::Builder &CustomScalarField2::Builder::withDerivativeResolution(double resolution) {
    _resolution = resolution;
    return *this;
}

CustomScalarField2 CustomScalarField2::Builder::build() const {
    if (_customLaplacianFunction) {
        return {_customFunction, _customGradientFunction, _customLaplacianFunction};
    } else {
        return {_customFunction, _customGradientFunction, _resolution};
    }
}

CustomScalarField2Ptr CustomScalarField2::Builder::makeShared() const {
    if (_customLaplacianFunction) {
        return {new CustomScalarField2(_customFunction, _customGradientFunction, _customLaplacianFunction),
                [](CustomScalarField2 *obj) { delete obj; }};
    } else {
        return {new CustomScalarField2(_customFunction, _customGradientFunction, _resolution),
                [](CustomScalarField2 *obj) { delete obj; }};
    }
}
