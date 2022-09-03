// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/custom_scalar_field3.h"

#include <utility>

using namespace vox;

CustomScalarField3::CustomScalarField3(std::function<double(const Point3D &)> customFunction,
                                       double derivativeResolution)
    : _customFunction(std::move(customFunction)), _resolution(derivativeResolution) {}

CustomScalarField3::CustomScalarField3(std::function<double(const Point3D &)> customFunction,
                                       std::function<Vector3D(const Point3D &)> customGradientFunction,
                                       double derivativeResolution)
    : _customFunction(std::move(customFunction)),
      _customGradientFunction(std::move(customGradientFunction)),
      _resolution(derivativeResolution) {}

CustomScalarField3::CustomScalarField3(std::function<double(const Point3D &)> customFunction,
                                       std::function<Vector3D(const Point3D &)> customGradientFunction,
                                       std::function<double(const Point3D &)> customLaplacianFunction)
    : _customFunction(std::move(customFunction)),
      _customGradientFunction(std::move(customGradientFunction)),
      _customLaplacianFunction(std::move(customLaplacianFunction)),
      _resolution(1e-3) {}

double CustomScalarField3::sample(const Point3D &x) const { return _customFunction(x); }

std::function<double(const Point3D &)> CustomScalarField3::sampler() const { return _customFunction; }

Vector3D CustomScalarField3::gradient(const Point3D &x) const {
    if (_customGradientFunction) {
        return _customGradientFunction(x);
    } else {
        double left = _customFunction(x - Vector3D(0.5 * _resolution, 0.0, 0.0));
        double right = _customFunction(x + Vector3D(0.5 * _resolution, 0.0, 0.0));
        double bottom = _customFunction(x - Vector3D(0.0, 0.5 * _resolution, 0.0));
        double top = _customFunction(x + Vector3D(0.0, 0.5 * _resolution, 0.0));
        double back = _customFunction(x - Vector3D(0.0, 0.0, 0.5 * _resolution));
        double front = _customFunction(x + Vector3D(0.0, 0.0, 0.5 * _resolution));

        return {(right - left) / _resolution, (top - bottom) / _resolution, (front - back) / _resolution};
    }
}

double CustomScalarField3::laplacian(const Point3D &x) const {
    if (_customLaplacianFunction) {
        return _customLaplacianFunction(x);
    } else {
        double center = _customFunction(x);
        double left = _customFunction(x - Vector3D(0.5 * _resolution, 0.0, 0.0));
        double right = _customFunction(x + Vector3D(0.5 * _resolution, 0.0, 0.0));
        double bottom = _customFunction(x - Vector3D(0.0, 0.5 * _resolution, 0.0));
        double top = _customFunction(x + Vector3D(0.0, 0.5 * _resolution, 0.0));
        double back = _customFunction(x - Vector3D(0.0, 0.0, 0.5 * _resolution));
        double front = _customFunction(x + Vector3D(0.0, 0.0, 0.5 * _resolution));

        return (left + right + bottom + top + back + front - 6.0 * center) / (_resolution * _resolution);
    }
}

CustomScalarField3::Builder CustomScalarField3::builder() { return {}; }

CustomScalarField3::Builder &CustomScalarField3::Builder::withFunction(
        const std::function<double(const Point3D &)> &func) {
    _customFunction = func;
    return *this;
}

CustomScalarField3::Builder &CustomScalarField3::Builder::withGradientFunction(
        const std::function<Vector3D(const Point3D &)> &func) {
    _customGradientFunction = func;
    return *this;
}

CustomScalarField3::Builder &CustomScalarField3::Builder::withLaplacianFunction(
        const std::function<double(const Point3D &)> &func) {
    _customLaplacianFunction = func;
    return *this;
}

CustomScalarField3::Builder &CustomScalarField3::Builder::withDerivativeResolution(double resolution) {
    _resolution = resolution;
    return *this;
}

CustomScalarField3 CustomScalarField3::Builder::build() const {
    if (_customLaplacianFunction) {
        return {_customFunction, _customGradientFunction, _customLaplacianFunction};
    } else {
        return {_customFunction, _customGradientFunction, _resolution};
    }
}

CustomScalarField3Ptr CustomScalarField3::Builder::makeShared() const {
    if (_customLaplacianFunction) {
        return {new CustomScalarField3(_customFunction, _customGradientFunction, _customLaplacianFunction),
                [](CustomScalarField3 *obj) { delete obj; }};
    } else {
        return {new CustomScalarField3(_customFunction, _customGradientFunction, _resolution),
                [](CustomScalarField3 *obj) { delete obj; }};
    }
}
