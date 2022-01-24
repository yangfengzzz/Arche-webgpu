//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef INCLUDE_VOX_NEAREST_NEIGHBOR_QUERY_ENGINE2_H_
#define INCLUDE_VOX_NEAREST_NEIGHBOR_QUERY_ENGINE2_H_

#include "constants.h"
#include "point2.h"

#include <functional>

namespace vox {

//! Nearest neighbor query result.
template<typename T>
struct NearestNeighborQueryResult2 {
    const T *item = nullptr;
    double distance = kMaxD;
};

//! Nearest neighbor distance measure function.
template<typename T>
using NearestNeighborDistanceFunc2 =
std::function<double(const T &, const Point2D &)>;

//! Abstract base class for 2-D nearest neigbor query engine.
template<typename T>
class NearestNeighborQueryEngine2 {
public:
    //! Returns the nearest neighbor for given point and distance measure
    //! function.
    virtual NearestNeighborQueryResult2<T>
    nearest(const Point2D &pt, const NearestNeighborDistanceFunc2<T> &distanceFunc) const = 0;
};

}  // namespace vox

#endif  // INCLUDE_VOX_NEAREST_NEIGHBOR_QUERY_ENGINE2_H_
