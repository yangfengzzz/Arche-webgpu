// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/vector_field2.h"

using namespace vox;

VectorField2::VectorField2() = default;

VectorField2::~VectorField2() = default;

double VectorField2::divergence(const Point2D &) const { return 0.0; }

double VectorField2::curl(const Point2D &) const { return 0.0; }

std::function<Vector2D(const Point2D &)> VectorField2::sampler() const {
    const VectorField2 *self = this;
    return [self](const Point2D &x) -> Vector2D { return self->sample(x); };
}
