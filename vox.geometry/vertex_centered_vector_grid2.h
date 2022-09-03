//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <utility>

#include "vox.geometry/collocated_vector_grid2.h"

namespace vox {

//!
//! \brief 2-D Vertex-centered vector grid structure.
//!
//! This class represents 2-D vertex-centered vector grid which extends
//! CollocatedVectorGrid2. As its name suggests, the class defines the data
//! point at the grid vertices (corners). Thus, A x B grid resolution will have
//! (A+1) x (B+1) data points.
//!
class VertexCenteredVectorGrid2 final : public CollocatedVectorGrid2 {
public:
    JET_GRID2_TYPE_NAME(VertexCenteredVectorGrid2)

    class Builder;

    //! Constructs zero-sized grid.
    VertexCenteredVectorGrid2();

    //! Constructs a grid with given resolution, grid spacing, origin and
    //! initial value.
    VertexCenteredVectorGrid2(size_t resolutionX,
                              size_t resolutionY,
                              double gridSpacingX = 1.0,
                              double gridSpacingY = 1.0,
                              double originX = 0.0,
                              double originY = 0.0,
                              double initialValueU = 0.0,
                              double initialValueV = 0.0);

    //! Constructs a grid with given resolution, grid spacing, origin and
    //! initial value.
    explicit VertexCenteredVectorGrid2(const Size2 &resolution,
                                       const Vector2D &gridSpacing = Vector2D(1.0, 1.0),
                                       const Point2D &origin = Point2D(),
                                       const Vector2D &initialValue = Vector2D());

    //! Copy constructor.
    VertexCenteredVectorGrid2(const VertexCenteredVectorGrid2 &other);

    //! Returns the actual data point size.
    [[nodiscard]] Size2 dataSize() const override;

    //! Returns data position for the grid point at (0, 0).
    //! Note that this is different from origin() since origin() returns
    //! the lower corner point of the bounding box.
    [[nodiscard]] Point2D dataOrigin() const override;

    //!
    //! \brief Swaps the contents with the given \p other grid.
    //!
    //! This function swaps the contents of the grid instance with the given
    //! grid object \p other only if \p other has the same type with this grid.
    //!
    void swap(Grid2 *other) override;

    //! Sets the contents with the given \p other grid.
    void set(const VertexCenteredVectorGrid2 &other);

    //! Sets the contents with the given \p other grid.
    VertexCenteredVectorGrid2 &operator=(const VertexCenteredVectorGrid2 &other);

    //! Fills the grid with given value.
    void fill(const Vector2D &value, ExecutionPolicy policy = ExecutionPolicy::kParallel) override;

    //! Fills the grid with given function.
    void fill(const std::function<Vector2D(const Point2D &)> &func,
              ExecutionPolicy policy = ExecutionPolicy::kParallel) override;

    //! Returns the copy of the grid instance.
    [[nodiscard]] std::shared_ptr<VectorGrid2> clone() const override;

    //! Returns builder fox VertexCenteredVectorGrid2.
    static Builder builder();
};

//! Shared pointer for the VertexCenteredVectorGrid2 type.
typedef std::shared_ptr<VertexCenteredVectorGrid2> VertexCenteredVectorGrid2Ptr;

//! A grid builder class that returns 2-D vertex-centered vector grid.
class VertexCenteredVectorGrid2::Builder final : public VectorGridBuilder2 {
public:
    //! Returns builder with resolution.
    Builder &withResolution(const Size2 &resolution);

    //! Returns builder with resolution.
    Builder &withResolution(size_t resolutionX, size_t resolutionY);

    //! Returns builder with grid spacing.
    Builder &withGridSpacing(const Vector2D &gridSpacing);

    //! Returns builder with grid spacing.
    Builder &withGridSpacing(double gridSpacingX, double gridSpacingY);

    //! Returns builder with grid origin.
    Builder &withOrigin(const Point2D &gridOrigin);

    //! Returns builder with grid origin.
    Builder &withOrigin(double gridOriginX, double gridOriginY);

    //! Returns builder with initial value.
    Builder &withInitialValue(const Vector2D &initialVal);

    //! Returns builder with initial value.
    Builder &withInitialValue(double initialValX, double initialValY);

    //! Builds VertexCenteredVectorGrid2 instance.
    [[nodiscard]] VertexCenteredVectorGrid2 build() const;

    //! Builds shared pointer of VertexCenteredVectorGrid2 instance.
    [[nodiscard]] VertexCenteredVectorGrid2Ptr makeShared() const;

    //!
    //! \brief Builds shared pointer of VertexCenteredVectorGrid2 instance.
    //!
    //! This is an overriding function that implements VectorGridBuilder2.
    //!
    [[nodiscard]] VectorGrid2Ptr build(const Size2 &resolution,
                                       const Vector2D &gridSpacing,
                                       const Point2D &gridOrigin,
                                       const Vector2D &initialVal) const override;

private:
    Size2 _resolution{1, 1};
    Vector2D _gridSpacing{1, 1};
    Point2D _gridOrigin{0, 0};
    Vector2D _initialVal{0, 0};
};

}  // namespace vox