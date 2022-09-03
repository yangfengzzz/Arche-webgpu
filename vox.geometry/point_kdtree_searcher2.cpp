// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#ifdef _MSC_VER
#pragma warning(disable : 4244)
#endif

#include "vox.geometry/point_kdtree_searcher2.h"

#include <numeric>

#include "vox.base/private_helpers.h"
#include "vox.math/bounding_box2.h"

using namespace vox;

PointKdTreeSearcher2::PointKdTreeSearcher2() = default;

PointKdTreeSearcher2::PointKdTreeSearcher2(const PointKdTreeSearcher2& other) { set(other); }

void PointKdTreeSearcher2::build(const ConstArrayAccessor1<Point2D>& points) { _tree.build(points); }

void PointKdTreeSearcher2::forEachNearbyPoint(const Point2D& origin,
                                              double radius,
                                              const ForEachNearbyPointFunc& callback) const {
    _tree.forEachNearbyPoint(origin, radius, callback);
}

bool PointKdTreeSearcher2::hasNearbyPoint(const Point2D& origin, double radius) const {
    return _tree.hasNearbyPoint(origin, radius);
}

PointNeighborSearcher2Ptr PointKdTreeSearcher2::clone() const {return CLONE_W_CUSTOM_DELETER(PointKdTreeSearcher2)}

PointKdTreeSearcher2& PointKdTreeSearcher2::operator=(const PointKdTreeSearcher2& other) {
    set(other);
    return *this;
}

void PointKdTreeSearcher2::set(const PointKdTreeSearcher2& other) { _tree = other._tree; }

PointKdTreeSearcher2::Builder PointKdTreeSearcher2::builder() { return Builder{}; }

//

PointKdTreeSearcher2 PointKdTreeSearcher2::Builder::build() const { return PointKdTreeSearcher2{}; }

PointKdTreeSearcher2Ptr PointKdTreeSearcher2::Builder::makeShared() const {
    return {new PointKdTreeSearcher2, [](PointKdTreeSearcher2* obj) { delete obj; }};
}

PointNeighborSearcher2Ptr PointKdTreeSearcher2::Builder::buildPointNeighborSearcher() const { return makeShared(); }
