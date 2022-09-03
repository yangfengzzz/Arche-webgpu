//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>
#include <vector>

#include "vox.base/parallel.h"
#include "vox.geometry/array_accessor2.h"
#include "vox.geometry/grid2.h"
#include "vox.geometry/vector_field2.h"

namespace vox {

//! Abstract base class for 2-D vector grid structure.
class VectorGrid2 : public VectorField2, public Grid2 {
public:
    //! Read-write array accessor type.
    typedef ArrayAccessor2<Vector2D> VectorDataAccessor;

    //! Read-only array accessor type.
    typedef ConstArrayAccessor2<Vector2D> ConstVectorDataAccessor;

    //! Constructs an empty grid.
    VectorGrid2();

    //! Default destructor.
    ~VectorGrid2() override;

    //! Clears the contents of the grid.
    void clear();

    //! Resizes the grid using given parameters.
    void resize(size_t resolutionX,
                size_t resolutionY,
                double gridSpacingX = 1.0,
                double gridSpacingY = 1.0,
                double originX = 0.0,
                double originY = 0.0,
                double initialValueX = 0.0,
                double initialValueY = 0.0);

    //! Resizes the grid using given parameters.
    void resize(const Size2 &resolution,
                const Vector2D &gridSpacing = Vector2D(1, 1),
                const Point2D &origin = Point2D(),
                const Vector2D &initialValue = Vector2D());

    //! Resizes the grid using given parameters.
    void resize(double gridSpacingX, double gridSpacingY, double originX, double originY);

    //! Resizes the grid using given parameters.
    void resize(const Vector2D &gridSpacing, const Point2D &origin);

    //! Fills the grid with given value.
    virtual void fill(const Vector2D &value, ExecutionPolicy policy = ExecutionPolicy::kParallel) = 0;

    //! Fills the grid with given position-to-value mapping function.
    virtual void fill(const std::function<Vector2D(const Point2D &)> &func,
                      ExecutionPolicy policy = ExecutionPolicy::kParallel) = 0;

    //! Returns the copy of the grid instance.
    [[nodiscard]] virtual std::shared_ptr<VectorGrid2> clone() const = 0;

protected:
    //!
    //! \brief Invoked when the resizing happens.
    //!
    //! This callback function is called when the grid gets resized. The
    //! overriding class should allocate the internal storage based on its
    //! data layout scheme.
    //!
    virtual void onResize(const Size2 &resolution,
                          const Vector2D &gridSpacing,
                          const Point2D &origin,
                          const Vector2D &initialValue) = 0;
};

//! Shared pointer for the VectorGrid2 type.
typedef std::shared_ptr<VectorGrid2> VectorGrid2Ptr;

//! Abstract base class for 2-D vector grid builder.
class VectorGridBuilder2 {
public:
    //! Creates a builder.
    VectorGridBuilder2();

    //! Default destructor.
    virtual ~VectorGridBuilder2();

    //! Returns 2-D vector grid with given parameters.
    [[nodiscard]] virtual VectorGrid2Ptr build(const Size2 &resolution,
                                               const Vector2D &gridSpacing,
                                               const Point2D &gridOrigin,
                                               const Vector2D &initialVal) const = 0;
};

//! Shared pointer for the VectorGridBuilder2 type.
typedef std::shared_ptr<VectorGridBuilder2> VectorGridBuilder2Ptr;

}  // namespace vox
