// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/constant_scalar_field3.h"

using namespace vox;

ConstantScalarField3::ConstantScalarField3(double value) : _value(value) {}

double ConstantScalarField3::sample(const Point3D& x) const { return _value; }

std::function<double(const Point3D&)> ConstantScalarField3::sampler() const {
    double value = _value;
    return [value](const Point3D&) -> double { return value; };
}

ConstantScalarField3::Builder ConstantScalarField3::builder() { return {}; }

ConstantScalarField3::Builder& ConstantScalarField3::Builder::withValue(double value) {
    _value = value;
    return *this;
}

ConstantScalarField3 ConstantScalarField3::Builder::build() const { return ConstantScalarField3(_value); }

ConstantScalarField3Ptr ConstantScalarField3::Builder::makeShared() const {
    return {new ConstantScalarField3(_value), [](ConstantScalarField3* obj) { delete obj; }};
}
