//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>

#include "vox.geometry/array3.h"
#include "vox.geometry/array_samplers3.h"
#include "vox.geometry/vector_grid3.h"

namespace vox {

//! \brief Abstract base class for 3-D collocated vector grid structure.
class CollocatedVectorGrid3 : public VectorGrid3 {
public:
    //! Constructs an empty grid.
    CollocatedVectorGrid3();

    //! Default destructor.
    ~CollocatedVectorGrid3() override;

    //! Returns the actual data point size.
    [[nodiscard]] virtual Size3 dataSize() const = 0;

    //!
    //! \brief Returns data position for the grid point at (0, 0, 0).
    //!
    //! Note that this is different from origin() since origin() returns
    //! the lower corner point of the bounding box.
    //!
    [[nodiscard]] virtual Point3D dataOrigin() const = 0;

    //! Returns the grid data at given data point.
    const Vector3D &operator()(size_t i, size_t j, size_t k) const;

    //! Returns the grid data at given data point.
    Vector3D &operator()(size_t i, size_t j, size_t k);

    //! Returns divergence at data point location.
    [[nodiscard]] double divergenceAtDataPoint(size_t i, size_t j, size_t k) const;

    //! Returns curl at data point location.
    [[nodiscard]] Vector3D curlAtDataPoint(size_t i, size_t j, size_t k) const;

    //! Returns the read-write data array accessor.
    VectorDataAccessor dataAccessor();

    //! Returns the read-only data array accessor.
    [[nodiscard]] ConstVectorDataAccessor constDataAccessor() const;

    //! Returns the function that maps data point to its position.
    [[nodiscard]] DataPositionFunc dataPosition() const;

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

    // VectorField3 implementations

    //! Returns sampled value at given position \p x.
    [[nodiscard]] Vector3D sample(const Point3D &x) const override;

    //! Returns divergence at given position \p x.
    [[nodiscard]] double divergence(const Point3D &x) const override;

    //! Returns curl at given position \p x.
    [[nodiscard]] Vector3D curl(const Point3D &x) const override;

    //!
    //! \brief Returns the sampler function.
    //!
    //! This function returns the data sampler function object. The sampling
    //! function is linear.
    //!
    [[nodiscard]] std::function<Vector3D(const Point3D &)> sampler() const override;

protected:
    //! Swaps the data storage and predefined samplers with given grid.
    void swapCollocatedVectorGrid(CollocatedVectorGrid3 *other);

    //! Sets the data storage and predefined samplers with given grid.
    void setCollocatedVectorGrid(const CollocatedVectorGrid3 &other);

    //! Fetches the data into a continuous linear array.
    void getData(std::vector<double> *data) const override;

    //! Sets the data from a continuous linear array.
    void setData(const std::vector<double> &data) override;

private:
    Array3<Vector3D> _data;
    LinearArraySampler3<Vector3D, double> _linearSampler;
    std::function<Vector3D(const Point3D &)> _sampler;

    void onResize(const Size3 &resolution,
                  const Vector3D &gridSpacing,
                  const Point3D &origin,
                  const Vector3D &initialValue) final;

    void resetSampler();
};

//! Shared pointer for the CollocatedVectorGrid3 type.
typedef std::shared_ptr<CollocatedVectorGrid3> CollocatedVectorGrid3Ptr;

}  // namespace vox