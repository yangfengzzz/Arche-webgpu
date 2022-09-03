//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>
#include <memory>

#include "vox.geometry/field3.h"
#include "vox.math/point3.h"

namespace vox {

//! Abstract base class for 3-D vector field.
class VectorField3 : public Field3 {
public:
    //! Default constructor.
    VectorField3();

    //! Default destructor.
    ~VectorField3() override;

    //! Returns sampled value at given position \p x.
    [[nodiscard]] virtual Vector3D sample(const Point3D &x) const = 0;

    //! Returns divergence at given position \p x.
    [[nodiscard]] virtual double divergence(const Point3D &x) const;

    //! Returns curl at given position \p x.
    [[nodiscard]] virtual Vector3D curl(const Point3D &x) const;

    //! Returns sampler function object.
    [[nodiscard]] virtual std::function<Vector3D(const Point3D &)> sampler() const;
};

//! Shared pointer for the VectorField3 type.
typedef std::shared_ptr<VectorField3> VectorField3Ptr;

}  // namespace vox