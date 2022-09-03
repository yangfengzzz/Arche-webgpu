// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/constant_vector_field2.h"

using namespace vox;

ConstantVectorField2::ConstantVectorField2(const Vector2D& value) : _value(value) {}

Vector2D ConstantVectorField2::sample(const Point2D& x) const { return _value; }

std::function<Vector2D(const Point2D&)> ConstantVectorField2::sampler() const {
    return [this](const Point2D&) -> Vector2D { return _value; };
}

ConstantVectorField2::Builder ConstantVectorField2::builder() { return {}; }

ConstantVectorField2::Builder& ConstantVectorField2::Builder::withValue(const Vector2D& value) {
    _value = value;
    return *this;
}

ConstantVectorField2 ConstantVectorField2::Builder::build() const { return ConstantVectorField2(_value); }

ConstantVectorField2Ptr ConstantVectorField2::Builder::makeShared() const {
    return {new ConstantVectorField2(_value), [](ConstantVectorField2* obj) { delete obj; }};
}
