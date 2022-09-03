// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/constant_scalar_field2.h"

using namespace vox;

ConstantScalarField2::ConstantScalarField2(double value) : _value(value) {}

double ConstantScalarField2::sample(const Point2D& x) const { return _value; }

std::function<double(const Point2D&)> ConstantScalarField2::sampler() const {
    double value = _value;
    return [value](const Point2D&) -> double { return value; };
}

ConstantScalarField2::Builder ConstantScalarField2::builder() { return {}; }

ConstantScalarField2::Builder& ConstantScalarField2::Builder::withValue(double value) {
    _value = value;
    return *this;
}

ConstantScalarField2 ConstantScalarField2::Builder::build() const { return ConstantScalarField2(_value); }

ConstantScalarField2Ptr ConstantScalarField2::Builder::makeShared() const {
    return {new ConstantScalarField2(_value), [](ConstantScalarField2* obj) { delete obj; }};
}
