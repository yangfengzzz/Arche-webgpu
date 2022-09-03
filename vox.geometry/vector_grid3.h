//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>
#include <vector>

#include "vox.geometry/array_accessor3.h"
#include "vox.geometry/grid3.h"
#include "vox.geometry/vector_field3.h"

namespace vox {

//! Abstract base class for 3-D vector grid structure.
class VectorGrid3 : public VectorField3, public Grid3 {
public:
    //! Read-write array accessor type.
    typedef ArrayAccessor3<Vector3D> VectorDataAccessor;

    //! Read-only array accessor type.
    typedef ConstArrayAccessor3<Vector3D> ConstVectorDataAccessor;

    //! Constructs an empty grid.
    VectorGrid3();

    //! Default destructor.
    ~VectorGrid3() override;

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
                double initialValueX = 0.0,
                double initialValueY = 0.0,
                double initialValueZ = 0.0);

    //! Resizes the grid using given parameters.
    void resize(const Size3 &resolution,
                const Vector3D &gridSpacing = Vector3D(1, 1, 1),
                const Point3D &origin = Point3D(),
                const Vector3D &initialValue = Vector3D());

    //! Resizes the grid using given parameters.
    void resize(double gridSpacingX,
                double gridSpacingY,
                double gridSpacingZ,
                double originX,
                double originY,
                double originZ);

    //! Resizes the grid using given parameters.
    void resize(const Vector3D &gridSpacing, const Point3D &origin);

    //! Fills the grid with given value.
    virtual void fill(const Vector3D &value, ExecutionPolicy policy = ExecutionPolicy::kParallel) = 0;

    //! Fills the grid with given position-to-value mapping function.
    virtual void fill(const std::function<Vector3D(const Point3D &)> &func,
                      ExecutionPolicy policy = ExecutionPolicy::kParallel) = 0;

    //! Returns the copy of the grid instance.
    [[nodiscard]] virtual std::shared_ptr<VectorGrid3> clone() const = 0;

protected:
    //!
    //! \brief Invoked when the resizing happens.
    //!
    //! This callback function is called when the grid gets resized. The
    //! overriding class should allocate the internal storage based on its
    //! data layout scheme.
    //!
    virtual void onResize(const Size3 &resolution,
                          const Vector3D &gridSpacing,
                          const Point3D &origin,
                          const Vector3D &initialValue) = 0;
};

//! Shared pointer for the VectorGrid3 type.
typedef std::shared_ptr<VectorGrid3> VectorGrid3Ptr;

//! Abstract base class for 3-D vector grid builder.
class VectorGridBuilder3 {
public:
    //! Creates a builder.
    VectorGridBuilder3();

    //! Default destructor.
    virtual ~VectorGridBuilder3();

    //! Returns 3-D vector grid with given parameters.
    [[nodiscard]] virtual VectorGrid3Ptr build(const Size3 &resolution,
                                               const Vector3D &gridSpacing,
                                               const Point3D &gridOrigin,
                                               const Vector3D &initialVal) const = 0;
};

//! Shared pointer for the VectorGridBuilder3 type.
typedef std::shared_ptr<VectorGridBuilder3> VectorGridBuilder3Ptr;

}  // namespace vox