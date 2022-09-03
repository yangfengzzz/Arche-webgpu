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

//! Abstract base class for 3-D scalar field.
class ScalarField3 : public Field3 {
public:
    //! Default constructor.
    ScalarField3();

    //! Default destructor.
    ~ScalarField3() override;

    //! Returns sampled value at given position \p x.
    [[nodiscard]] virtual double sample(const Point3D &x) const = 0;

    //! Returns gradient vector at given position \p x.
    [[nodiscard]] virtual Vector3D gradient(const Point3D &x) const;

    //! Returns Laplacian at given position \p x.
    [[nodiscard]] virtual double laplacian(const Point3D &x) const;

    //! Returns sampler function object.
    [[nodiscard]] virtual std::function<double(const Point3D &)> sampler() const;
};

//! Shared pointer for the ScalarField3 type.
typedef std::shared_ptr<ScalarField3> ScalarField3Ptr;

}  // namespace vox