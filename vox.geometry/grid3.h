//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "vox.math/bounding_box3.h"
#include "vox.math/size3.h"

namespace vox {

//!
//! \brief Abstract base class for 3-D cartesian grid structure.
//!
//! This class represents 3-D cartesian grid structure. This class is an
//! abstract base class and does not store any data. The class only stores the
//! shape of the grid. The grid structure is axis-aligned and can have different
//! grid spacing per axis.
//!
class Grid3 {
public:
    //! Function type for mapping data index to actual position.
    typedef std::function<Point3D(size_t, size_t, size_t)> DataPositionFunc;

    //! Constructs an empty grid.
    Grid3();

    //! Default destructor.
    virtual ~Grid3();

    //! Returns the type name of derived grid.
    [[nodiscard]] virtual std::string typeName() const = 0;

    //! Returns the grid resolution.
    [[nodiscard]] const Size3 &resolution() const;

    //! Returns the grid origin.
    [[nodiscard]] const Point3D &origin() const;

    //! Returns the grid spacing.
    [[nodiscard]] const Vector3D &gridSpacing() const;

    //! Returns the bounding box of the grid.
    [[nodiscard]] const BoundingBox3D &boundingBox() const;

    //! Returns the function that maps grid index to the cell-center position.
    [[nodiscard]] DataPositionFunc cellCenterPosition() const;

    //!
    //! \brief Invokes the given function \p func for each grid cell.
    //!
    //! This function invokes the given function object \p func for each grid
    //! cell in serial manner. The input parameters are i, j, and k indices of a
    //! grid cell. The order of execution is i-first, j-next, k-last.
    //!
    void forEachCellIndex(const std::function<void(size_t, size_t, size_t)> &func) const;

    //!
    //! \brief Invokes the given function \p func for each grid cell parallelly.
    //!
    //! This function invokes the given function object \p func for each grid
    //! cell in parallel manner. The input parameters are i, j, and k indices of
    //! a grid cell. The order of execution can be arbitrary since it's
    //! multi-threaded.
    //!
    void parallelForEachCellIndex(const std::function<void(size_t, size_t, size_t)> &func) const;

    //! Returns true if resolution, grid-spacing and origin are same.
    [[nodiscard]] bool hasSameShape(const Grid3 &other) const;

    //! Swaps the data with other grid.
    virtual void swap(Grid3 *other) = 0;

protected:
    //! Sets the size parameters including the resolution, grid spacing, and
    //! origin.
    void setSizeParameters(const Size3 &resolution, const Vector3D &gridSpacing, const Point3D &origin);

    //! Swaps the size parameters with given grid \p other.
    void swapGrid(Grid3 *other);

    //! Sets the size parameters with given grid \p other.
    void setGrid(const Grid3 &other);

    //! Fetches the data into a continuous linear array.
    virtual void getData(std::vector<double> *data) const = 0;

    //! Sets the data from a continuous linear array.
    virtual void setData(const std::vector<double> &data) = 0;

private:
    Size3 _resolution;
    Vector3D _gridSpacing = Vector3D(1, 1, 1);
    Point3D _origin;
    BoundingBox3D _boundingBox = BoundingBox3D(Point3D(), Point3D());
};

typedef std::shared_ptr<Grid3> Grid3Ptr;

#define JET_GRID3_TYPE_NAME(DerivedClassName) \
    std::string typeName() const override { return #DerivedClassName; }

}  // namespace vox