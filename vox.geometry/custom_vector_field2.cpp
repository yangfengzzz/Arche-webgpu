// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/custom_vector_field2.h"

#include <utility>

using namespace vox;

CustomVectorField2::CustomVectorField2(std::function<Vector2D(const Point2D &)> customFunction,
                                       double derivativeResolution)
    : _customFunction(std::move(customFunction)), _resolution(derivativeResolution) {}

CustomVectorField2::CustomVectorField2(std::function<Vector2D(const Point2D &)> customFunction,
                                       std::function<double(const Point2D &)> customDivergenceFunction,
                                       double derivativeResolution)
    : _customFunction(std::move(customFunction)),
      _customDivergenceFunction(std::move(customDivergenceFunction)),
      _resolution(derivativeResolution) {}

CustomVectorField2::CustomVectorField2(std::function<Vector2D(const Point2D &)> customFunction,
                                       std::function<double(const Point2D &)> customDivergenceFunction,
                                       std::function<double(const Point2D &)> customCurlFunction)
    : _customFunction(std::move(customFunction)),
      _customDivergenceFunction(std::move(customDivergenceFunction)),
      _customCurlFunction(std::move(customCurlFunction)) {}

Vector2D CustomVectorField2::sample(const Point2D &x) const { return _customFunction(x); }

std::function<Vector2D(const Point2D &)> CustomVectorField2::sampler() const { return _customFunction; }

double CustomVectorField2::divergence(const Point2D &x) const {
    if (_customDivergenceFunction) {
        return _customDivergenceFunction(x);
    } else {
        double left = _customFunction(x - Vector2D(0.5 * _resolution, 0.0)).x;
        double right = _customFunction(x + Vector2D(0.5 * _resolution, 0.0)).x;
        double bottom = _customFunction(x - Vector2D(0.0, 0.5 * _resolution)).y;
        double top = _customFunction(x + Vector2D(0.0, 0.5 * _resolution)).y;

        return (right - left + top - bottom) / _resolution;
    }
}

double CustomVectorField2::curl(const Point2D &x) const {
    if (_customCurlFunction) {
        return _customCurlFunction(x);
    } else {
        double left = _customFunction(x - Vector2D(0.5 * _resolution, 0.0)).y;
        double right = _customFunction(x + Vector2D(0.5 * _resolution, 0.0)).y;
        double bottom = _customFunction(x - Vector2D(0.0, 0.5 * _resolution)).x;
        double top = _customFunction(x + Vector2D(0.0, 0.5 * _resolution)).x;

        return (top - bottom - right + left) / _resolution;
    }
}

CustomVectorField2::Builder CustomVectorField2::builder() { return {}; }

CustomVectorField2::Builder &CustomVectorField2::Builder::withFunction(
        const std::function<Vector2D(const Point2D &)> &func) {
    _customFunction = func;
    return *this;
}

CustomVectorField2::Builder &CustomVectorField2::Builder::withDivergenceFunction(
        const std::function<double(const Point2D &)> &func) {
    _customDivergenceFunction = func;
    return *this;
}

CustomVectorField2::Builder &CustomVectorField2::Builder::withCurlFunction(
        const std::function<double(const Point2D &)> &func) {
    _customCurlFunction = func;
    return *this;
}

CustomVectorField2::Builder &CustomVectorField2::Builder::withDerivativeResolution(double resolution) {
    _resolution = resolution;
    return *this;
}

CustomVectorField2 CustomVectorField2::Builder::build() const {
    if (_customCurlFunction) {
        return {_customFunction, _customDivergenceFunction, _customCurlFunction};
    } else {
        return {_customFunction, _customDivergenceFunction, _resolution};
    }
}

CustomVectorField2Ptr CustomVectorField2::Builder::makeShared() const {
    if (_customCurlFunction) {
        return {new CustomVectorField2(_customFunction, _customDivergenceFunction, _customCurlFunction),
                [](CustomVectorField2 *obj) { delete obj; }};
    } else {
        return {new CustomVectorField2(_customFunction, _customDivergenceFunction, _resolution),
                [](CustomVectorField2 *obj) { delete obj; }};
    }
}
