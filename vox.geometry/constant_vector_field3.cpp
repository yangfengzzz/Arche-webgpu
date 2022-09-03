// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/constant_vector_field3.h"

using namespace vox;

ConstantVectorField3::ConstantVectorField3(const Vector3D& value) : _value(value) {}

Vector3D ConstantVectorField3::sample(const Point3D& x) const { return _value; }

std::function<Vector3D(const Point3D&)> ConstantVectorField3::sampler() const {
    return [this](const Point3D&) -> Vector3D { return _value; };
}

ConstantVectorField3::Builder ConstantVectorField3::builder() { return {}; }

ConstantVectorField3::Builder& ConstantVectorField3::Builder::withValue(const Vector3D& value) {
    _value = value;
    return *this;
}

ConstantVectorField3 ConstantVectorField3::Builder::build() const { return ConstantVectorField3(_value); }

ConstantVectorField3Ptr ConstantVectorField3::Builder::makeShared() const {
    return {new ConstantVectorField3(_value), [](ConstantVectorField3* obj) { delete obj; }};
}
