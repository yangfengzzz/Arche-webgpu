//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.geometry/surface3.h"

namespace vox {

//! Abstract base class for 3-D implicit surface.
class ImplicitSurface3 : public Surface3 {
public:
    //! Default constructor.
    explicit ImplicitSurface3(const Transform3D &transform = Transform3D(), bool isNormalFlipped = false);

    //! Copy constructor.
    ImplicitSurface3(const ImplicitSurface3 &other);

    //! Default destructor.
    ~ImplicitSurface3() override;

    //! Returns signed distance from the given point \p otherPoint.
    [[nodiscard]] double signedDistance(const Point3D &otherPoint) const;

protected:
    //! Returns signed distance from the given point \p otherPoint in local
    //! space.
    [[nodiscard]] virtual double signedDistanceLocal(const Point3D &otherPoint) const = 0;

private:
    [[nodiscard]] double closestDistanceLocal(const Point3D &otherPoint) const override;

    [[nodiscard]] bool isInsideLocal(const Point3D &otherPoint) const override;
};

//! Shared pointer type for the ImplicitSurface3.
typedef std::shared_ptr<ImplicitSurface3> ImplicitSurface3Ptr;

}  // namespace vox