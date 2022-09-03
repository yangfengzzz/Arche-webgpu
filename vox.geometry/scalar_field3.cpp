// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/scalar_field3.h"

using namespace vox;

ScalarField3::ScalarField3() = default;

ScalarField3::~ScalarField3() = default;

Vector3D ScalarField3::gradient(const Point3D &) const { return {}; }

double ScalarField3::laplacian(const Point3D &) const { return 0.0; }

std::function<double(const Point3D &)> ScalarField3::sampler() const {
    const ScalarField3 *self = this;
    return [self](const Point3D &x) -> double { return self->sample(x); };
}
