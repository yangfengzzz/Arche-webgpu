// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/scalar_field2.h"

using namespace vox;

ScalarField2::ScalarField2() = default;

ScalarField2::~ScalarField2() = default;

Vector2D ScalarField2::gradient(const Point2D &) const { return {}; }

double ScalarField2::laplacian(const Point2D &) const { return 0.0; }

std::function<double(const Point2D &)> ScalarField2::sampler() const {
    const ScalarField2 *self = this;
    return [self](const Point2D &x) -> double { return self->sample(x); };
}
