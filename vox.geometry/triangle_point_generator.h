//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.geometry/point_generator2.h"

namespace vox {

//!
//! \brief Right triangle point generator.
//!
class TrianglePointGenerator final : public PointGenerator2 {
public:
    //!
    //! \brief Invokes \p callback function for each right triangle points
    //! inside \p boundingBox.
    //!
    //! This function iterates every right triangle points inside \p boundingBox
    //! where \p spacing is the size of the right triangle structure.
    //!
    void forEachPoint(const BoundingBox2D& boundingBox,
                      double spacing,
                      const std::function<bool(const Point2D&)>& callback) const override;
};

typedef std::shared_ptr<TrianglePointGenerator> TrianglePointGeneratorPtr;

}  // namespace vox
