//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.geometry/scalar_field3.h"

namespace vox {

//! 3-D constant scalar field.
class ConstantScalarField3 final : public ScalarField3 {
public:
    class Builder;

    //! Constructs a constant scalar field with given \p value.
    explicit ConstantScalarField3(double value);

    //! Returns the sampled value at given position \p x.
    [[nodiscard]] double sample(const Point3D& x) const override;

    //! Returns the sampler function.
    [[nodiscard]] std::function<double(const Point3D&)> sampler() const override;

    //! Returns builder fox ConstantScalarField3.
    static Builder builder();

private:
    double _value = 0.0;
};

//! Shared pointer for the ConstantScalarField3 type.
typedef std::shared_ptr<ConstantScalarField3> ConstantScalarField3Ptr;

//!
//! \brief Front-end to create ConstantScalarField3 objects step by step.
//!
class ConstantScalarField3::Builder final {
public:
    //! Returns builder with value.
    Builder& withValue(double value);

    //! Builds ConstantScalarField3.
    [[nodiscard]] ConstantScalarField3 build() const;

    //! Builds shared pointer of ConstantScalarField3 instance.
    [[nodiscard]] ConstantScalarField3Ptr makeShared() const;

private:
    double _value = 0.0;
};

}  // namespace vox