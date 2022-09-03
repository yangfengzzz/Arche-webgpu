//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <utility>

#include "vox.geometry/scalar_grid2.h"

namespace vox {

//!
//! \brief 2-D Cell-centered scalar grid structure.
//!
//! This class represents 2-D cell-centered scalar grid which extends
//! ScalarGrid2. As its name suggests, the class defines the data point at the
//! center of a grid cell. Thus, the dimension of data points are equal to the
//! dimension of the cells.
//!
class CellCenteredScalarGrid2 final : public ScalarGrid2 {
public:
    JET_GRID2_TYPE_NAME(CellCenteredScalarGrid2)

    class Builder;

    //! Constructs zero-sized grid.
    CellCenteredScalarGrid2();

    //! Constructs a grid with given resolution, grid spacing, origin and
    //! initial value.
    CellCenteredScalarGrid2(size_t resolutionX,
                            size_t resolutionY,
                            double gridSpacingX = 1.0,
                            double gridSpacingY = 1.0,
                            double originX = 0.0,
                            double originY = 0.0,
                            double initialValue = 0.0);

    //! Constructs a grid with given resolution, grid spacing, origin and
    //! initial value.
    explicit CellCenteredScalarGrid2(const Size2 &resolution,
                                     const Vector2D &gridSpacing = Vector2D(1.0, 1.0),
                                     const Point2D &origin = Point2D(),
                                     double initialValue = 0.0);

    //! Copy constructor.
    CellCenteredScalarGrid2(const CellCenteredScalarGrid2 &other);

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
    void set(const CellCenteredScalarGrid2 &other);

    //! Sets the contents with the given \p other grid.
    CellCenteredScalarGrid2 &operator=(const CellCenteredScalarGrid2 &other);

    //! Returns the copy of the grid instance.
    [[nodiscard]] std::shared_ptr<ScalarGrid2> clone() const override;

    //! Returns builder fox CellCenteredScalarGrid2.
    static Builder builder();
};

//! Shared pointer for the CellCenteredScalarGrid2 type.
typedef std::shared_ptr<CellCenteredScalarGrid2> CellCenteredScalarGrid2Ptr;

//!
//! \brief Front-end to create CellCenteredScalarGrid2 objects step by step.
//!
class CellCenteredScalarGrid2::Builder final : public ScalarGridBuilder2 {
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
    Builder &withInitialValue(double initialVal);

    //! Builds CellCenteredScalarGrid2 instance.
    [[nodiscard]] CellCenteredScalarGrid2 build() const;

    //! Builds shared pointer of CellCenteredScalarGrid2 instance.
    [[nodiscard]] CellCenteredScalarGrid2Ptr makeShared() const;

    //!
    //! \brief Builds shared pointer of CellCenteredScalarGrid2 instance.
    //!
    //! This is an overriding function that implements ScalarGridBuilder2.
    //!
    [[nodiscard]] ScalarGrid2Ptr build(const Size2 &resolution,
                                       const Vector2D &gridSpacing,
                                       const Point2D &gridOrigin,
                                       double initialVal) const override;

private:
    Size2 _resolution{1, 1};
    Vector2D _gridSpacing{1, 1};
    Point2D _gridOrigin{0, 0};
    double _initialVal = 0.0;
};

}  // namespace vox