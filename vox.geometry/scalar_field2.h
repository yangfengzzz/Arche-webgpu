//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>
#include <memory>

#include "vox.geometry/field2.h"
#include "vox.math/point2.h"

namespace vox {

//! Abstract base class for 2-D scalar field.
class ScalarField2 : public Field2 {
public:
    //! Default constructor.
    ScalarField2();

    //! Default destructor.
    ~ScalarField2() override;

    //! Returns sampled value at given position \p x.
    [[nodiscard]] virtual double sample(const Point2D &x) const = 0;

    //! Returns gradient vector at given position \p x.
    [[nodiscard]] virtual Vector2D gradient(const Point2D &x) const;

    //! Returns Laplacian at given position \p x.
    [[nodiscard]] virtual double laplacian(const Point2D &x) const;

    //! Returns sampler function object.
    [[nodiscard]] virtual std::function<double(const Point2D &)> sampler() const;
};

//! Shared pointer for the ScalarField2 type.
typedef std::shared_ptr<ScalarField2> ScalarField2Ptr;

}  // namespace vox