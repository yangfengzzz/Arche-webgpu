//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>

#include "vox.base/constants.h"
#include "vox.math/vector3.h"

namespace vox {

//! Nearest neighbor query result.
template <typename T>
struct NearestNeighborQueryResult3 {
    const T *item = nullptr;
    double distance = kMaxD;
};

//! Nearest neighbor distance measure function.
template <typename T>
using NearestNeighborDistanceFunc3 = std::function<double(const T &, const Point3D &)>;

//! Abstract base class for 3-D nearest neighbour query engine.
template <typename T>
class NearestNeighborQueryEngine3 {
public:
    //! Returns the nearest neighbor for given point and distance measure
    //! function.
    virtual NearestNeighborQueryResult3<T> nearest(const Point3D &pt,
                                                   const NearestNeighborDistanceFunc3<T> &distanceFunc) const = 0;
};

}  // namespace vox
