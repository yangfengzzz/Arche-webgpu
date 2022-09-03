//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <utility>

#include "vox.geometry/array3.h"
#include "vox.geometry/scalar_grid3.h"

namespace vox {

//!
//! \brief 3-D Vertex-centered scalar grid structure.
//!
//! This class represents 3-D vertex-centered scalar grid which extends
//! ScalarGrid3. As its name suggests, the class defines the data point at the
//! grid vertices (corners). Thus, A x B x C grid resolution will have
//! (A+1) x (B+1) x (C+1) data points.
//!
class VertexCenteredScalarGrid3 final : public ScalarGrid3 {
public:
    JET_GRID3_TYPE_NAME(VertexCenteredScalarGrid3)

    class Builder;

    //! Constructs zero-sized grid.
    VertexCenteredScalarGrid3();

    //! Constructs a grid with given resolution, grid spacing, origin and
    //! initial value.
    VertexCenteredScalarGrid3(size_t resolutionX,
                              size_t resolutionY,
                              size_t resolutionZ,
                              double gridSpacingX = 1.0,
                              double gridSpacingY = 1.0,
                              double gridSpacingZ = 1.0,
                              double originX = 0.0,
                              double originY = 0.0,
                              double originZ = 0.0,
                              double initialValue = 0.0);

    //! Constructs a grid with given resolution, grid spacing, origin and
    //! initial value.
    explicit VertexCenteredScalarGrid3(const Size3 &resolution,
                                       const Vector3D &gridSpacing = Vector3D(1.0, 1.0, 1.0),
                                       const Point3D &origin = Point3D(),
                                       double initialValue = 0.0);

    //! Copy constructor.
    VertexCenteredScalarGrid3(const VertexCenteredScalarGrid3 &other);

    //! Returns the actual data point size.
    [[nodiscard]] Size3 dataSize() const override;

    //! Returns data position for the grid point at (0, 0, 0).
    //! Note that this is different from origin() since origin() returns
    //! the lower corner point of the bounding box.
    [[nodiscard]] Point3D dataOrigin() const override;

    //! Returns the copy of the grid instance.
    [[nodiscard]] std::shared_ptr<ScalarGrid3> clone() const override;

    //!
    //! \brief Swaps the contents with the given \p other grid.
    //!
    //! This function swaps the contents of the grid instance with the given
    //! grid object \p other only if \p other has the same type with this grid.
    //!
    void swap(Grid3 *other) override;

    //! Sets the contents with the given \p other grid.
    void set(const VertexCenteredScalarGrid3 &other);

    //! Sets the contents with the given \p other grid.
    VertexCenteredScalarGrid3 &operator=(const VertexCenteredScalarGrid3 &other);

    //! Returns builder fox VertexCenteredScalarGrid3.
    static Builder builder();
};

//! Shared pointer for the VertexCenteredScalarGrid3 type.
typedef std::shared_ptr<VertexCenteredScalarGrid3> VertexCenteredScalarGrid3Ptr;

//! A grid builder class that returns 3-D vertex-centered scalar grid.
class VertexCenteredScalarGrid3::Builder final : public ScalarGridBuilder3 {
public:
    //! Returns builder with resolution.
    Builder &withResolution(const Size3 &resolution);

    //! Returns builder with resolution.
    Builder &withResolution(size_t resolutionX, size_t resolutionY, size_t resolutionZ);

    //! Returns builder with grid spacing.
    Builder &withGridSpacing(const Vector3D &gridSpacing);

    //! Returns builder with grid spacing.
    Builder &withGridSpacing(double gridSpacingX, double gridSpacingY, double gridSpacingZ);

    //! Returns builder with grid origin.
    Builder &withOrigin(const Point3D &gridOrigin);

    //! Returns builder with grid origin.
    Builder &withOrigin(double gridOriginX, double gridOriginY, double gridOriginZ);

    //! Returns builder with initial value.
    Builder &withInitialValue(double initialVal);

    //! Builds VertexCenteredScalarGrid3 instance.
    [[nodiscard]] VertexCenteredScalarGrid3 build() const;

    //! Builds shared pointer of VertexCenteredScalarGrid3 instance.
    [[nodiscard]] VertexCenteredScalarGrid3Ptr makeShared() const;

    //!
    //! \brief Builds shared pointer of VertexCenteredScalarGrid3 instance.
    //!
    //! This is an overriding function that implements ScalarGridBuilder3.
    //!
    [[nodiscard]] ScalarGrid3Ptr build(const Size3 &resolution,
                                       const Vector3D &gridSpacing,
                                       const Point3D &gridOrigin,
                                       double initialVal) const override;

private:
    Size3 _resolution{1, 1, 1};
    Vector3D _gridSpacing{1, 1, 1};
    Point3D _gridOrigin{0, 0, 0};
    double _initialVal = 0.0;
};

}  // namespace vox