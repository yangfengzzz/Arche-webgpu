//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>
#include <vector>

#include "vox.geometry/array3.h"
#include "vox.geometry/array_accessor3.h"
#include "vox.geometry/array_samplers3.h"
#include "vox.geometry/grid3.h"
#include "vox.geometry/scalar_field3.h"

namespace vox {

//! Abstract base class for 3-D scalar grid structure.
class ScalarGrid3 : public ScalarField3, public Grid3 {
public:
    //! Read-write array accessor type.
    typedef ArrayAccessor3<double> ScalarDataAccessor;

    //! Read-only array accessor type.
    typedef ConstArrayAccessor3<double> ConstScalarDataAccessor;

    //! Constructs an empty grid.
    ScalarGrid3();

    //! Default destructor.
    ~ScalarGrid3() override;

    //!
    //! \brief Returns the size of the grid data.
    //!
    //! This function returns the size of the grid data which is not necessarily
    //! equal to the grid resolution if the data is not stored at cell-center.
    //!
    [[nodiscard]] virtual Size3 dataSize() const = 0;

    //!
    //! \brief Returns the origin of the grid data.
    //!
    //! This function returns data position for the grid point at (0, 0, 0).
    //! Note that this is different from origin() since origin() returns
    //! the lower corner point of the bounding box.
    //!
    [[nodiscard]] virtual Point3D dataOrigin() const = 0;

    //! Returns the copy of the grid instance.
    [[nodiscard]] virtual std::shared_ptr<ScalarGrid3> clone() const = 0;

    //! Clears the contents of the grid.
    void clear();

    //! Resizes the grid using given parameters.
    void resize(size_t resolutionX,
                size_t resolutionY,
                size_t resolutionZ,
                double gridSpacingX = 1.0,
                double gridSpacingY = 1.0,
                double gridSpacingZ = 1.0,
                double originX = 0.0,
                double originY = 0.0,
                double originZ = 0.0,
                double initialValue = 0.0);

    //! Resizes the grid using given parameters.
    void resize(const Size3 &resolution,
                const Vector3D &gridSpacing = Vector3D(1, 1, 1),
                const Point3D &origin = Point3D(),
                double initialValue = 0.0);

    //! Resizes the grid using given parameters.
    void resize(double gridSpacingX,
                double gridSpacingY,
                double gridSpacingZ,
                double originX,
                double originY,
                double originZ);

    //! Resizes the grid using given parameters.
    void resize(const Vector3D &gridSpacing, const Point3D &origin);

    //! Returns the grid data at given data point.
    const double &operator()(size_t i, size_t j, size_t k) const;

    //! Returns the grid data at given data point.
    double &operator()(size_t i, size_t j, size_t k);

    //! Returns the gradient vector at given data point.
    [[nodiscard]] Vector3D gradientAtDataPoint(size_t i, size_t j, size_t k) const;

    //! Returns the Laplacian at given data point.
    [[nodiscard]] double laplacianAtDataPoint(size_t i, size_t j, size_t k) const;

    //! Returns the read-write data array accessor.
    ScalarDataAccessor dataAccessor();

    //! Returns the read-only data array accessor.
    [[nodiscard]] ConstScalarDataAccessor constDataAccessor() const;

    //! Returns the function that maps data point to its position.
    [[nodiscard]] DataPositionFunc dataPosition() const;

    //! Fills the grid with given value.
    void fill(double value, ExecutionPolicy policy = ExecutionPolicy::kParallel);

    //! Fills the grid with given position-to-value mapping function.
    void fill(const std::function<double(const Point3D &)> &func, ExecutionPolicy policy = ExecutionPolicy::kParallel);

    //!
    //! \brief Invokes the given function \p func for each data point.
    //!
    //! This function invokes the given function object \p func for each data
    //! point in serial manner. The input parameters are i and j indices of a
    //! data point. The order of execution is i-first, j-last.
    //!
    void forEachDataPointIndex(const std::function<void(size_t, size_t, size_t)> &func) const;

    //!
    //! \brief Invokes the given function \p func for each data point
    //! parallelly.
    //!
    //! This function invokes the given function object \p func for each data
    //! point in parallel manner. The input parameters are i and j indices of a
    //! data point. The order of execution can be arbitrary since it's
    //! multi-threaded.
    //!
    void parallelForEachDataPointIndex(const std::function<void(size_t, size_t, size_t)> &func) const;

    // ScalarField3 implementations

    //!
    //! \brief Returns the sampled value at given position \p x.
    //!
    //! This function returns the data sampled at arbitrary position \p x.
    //! The sampling function is linear.
    //!
    [[nodiscard]] double sample(const Point3D &x) const override;

    //!
    //! \brief Returns the sampler function.
    //!
    //! This function returns the data sampler function object. The sampling
    //! function is linear.
    //!
    [[nodiscard]] std::function<double(const Point3D &)> sampler() const override;

    //! Returns the gradient vector at given position \p x.
    [[nodiscard]] Vector3D gradient(const Point3D &x) const override;

    //! Returns the Laplacian at given position \p x.
    [[nodiscard]] double laplacian(const Point3D &x) const override;

protected:
    //! Swaps the data storage and predefined samplers with given grid.
    void swapScalarGrid(ScalarGrid3 *other);

    //! Sets the data storage and predefined samplers with given grid.
    void setScalarGrid(const ScalarGrid3 &other);

    //! Fetches the data into a continuous linear array.
    void getData(std::vector<double> *data) const override;

    //! Sets the data from a continuous linear array.
    void setData(const std::vector<double> &data) override;

private:
    Array3<double> _data;
    LinearArraySampler3<double, double> _linearSampler;
    std::function<double(const Point3D &)> _sampler;

    void resetSampler();
};

//! Shared pointer for the ScalarGrid3 type.
typedef std::shared_ptr<ScalarGrid3> ScalarGrid3Ptr;

//! Abstract base class for 3-D scalar grid builder.
class ScalarGridBuilder3 {
public:
    //! Creates a builder.
    ScalarGridBuilder3();

    //! Default destructor.
    virtual ~ScalarGridBuilder3();

    //! Returns 3-D scalar grid with given parameters.
    [[nodiscard]] virtual ScalarGrid3Ptr build(const Size3 &resolution,
                                               const Vector3D &gridSpacing,
                                               const Point3D &gridOrigin,
                                               double initialVal) const = 0;
};

//! Shared pointer for the ScalarGridBuilder3 type.
typedef std::shared_ptr<ScalarGridBuilder3> ScalarGridBuilder3Ptr;

}  // namespace vox