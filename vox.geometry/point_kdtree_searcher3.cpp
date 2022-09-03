// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#ifdef _MSC_VER
#pragma warning(disable : 4244)
#endif

#include "vox.geometry/point_kdtree_searcher3.h"

#include <numeric>

#include "vox.base/private_helpers.h"
#include "vox.math/bounding_box3.h"

using namespace vox;

PointKdTreeSearcher3::PointKdTreeSearcher3() = default;

PointKdTreeSearcher3::PointKdTreeSearcher3(const PointKdTreeSearcher3& other) { set(other); }

void PointKdTreeSearcher3::build(const ConstArrayAccessor1<Point3D>& points) { _tree.build(points); }

void PointKdTreeSearcher3::forEachNearbyPoint(const Point3D& origin,
                                              double radius,
                                              const ForEachNearbyPointFunc& callback) const {
    _tree.forEachNearbyPoint(origin, radius, callback);
}

bool PointKdTreeSearcher3::hasNearbyPoint(const Point3D& origin, double radius) const {
    return _tree.hasNearbyPoint(origin, radius);
}

PointNeighborSearcher3Ptr PointKdTreeSearcher3::clone() const {return CLONE_W_CUSTOM_DELETER(PointKdTreeSearcher3)}

PointKdTreeSearcher3& PointKdTreeSearcher3::operator=(const PointKdTreeSearcher3& other) {
    set(other);
    return *this;
}

void PointKdTreeSearcher3::set(const PointKdTreeSearcher3& other) { _tree = other._tree; }

PointKdTreeSearcher3::Builder PointKdTreeSearcher3::builder() { return Builder{}; }

//

PointKdTreeSearcher3 PointKdTreeSearcher3::Builder::build() const { return PointKdTreeSearcher3{}; }

PointKdTreeSearcher3Ptr PointKdTreeSearcher3::Builder::makeShared() const {
    return {new PointKdTreeSearcher3, [](PointKdTreeSearcher3* obj) { delete obj; }};
}

PointNeighborSearcher3Ptr PointKdTreeSearcher3::Builder::buildPointNeighborSearcher() const { return makeShared(); }
