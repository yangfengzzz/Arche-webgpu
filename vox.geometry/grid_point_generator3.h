//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.geometry/point_generator3.h"

namespace vox {

//!
//! \brief 3-D regular-grid point generator.
//!
class GridPointGenerator3 final : public PointGenerator3 {
public:
    //!
    //! \brief Invokes \p callback function for each regular grid points inside
    //! \p boundingBox.
    //!
    //! This function iterates every regular grid points inside \p boundingBox
    //! where \p spacing is the size of the unit cell of regular grid structure.
    //!
    void forEachPoint(const BoundingBox3D& boundingBox,
                      double spacing,
                      const std::function<bool(const Point3D&)>& callback) const override;
};

//! Shared pointer type for the GridPointGenerator3.
typedef std::shared_ptr<GridPointGenerator3> GridPointGenerator3Ptr;

}  // namespace vox
