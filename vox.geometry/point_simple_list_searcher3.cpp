// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/point_simple_list_searcher3.h"

#include <algorithm>

#include "vox.base/private_helpers.h"

using namespace vox;

PointSimpleListSearcher3::PointSimpleListSearcher3() = default;

PointSimpleListSearcher3::PointSimpleListSearcher3(const PointSimpleListSearcher3& other) { set(other); }

void PointSimpleListSearcher3::build(const ConstArrayAccessor1<Point3D>& points) {
    _points.resize(points.size());
    std::copy(points.data(), points.data() + points.size(), _points.begin());
}

void PointSimpleListSearcher3::forEachNearbyPoint(const Point3D& origin,
                                                  double radius,
                                                  const std::function<void(size_t, const Point3D&)>& callback) const {
    double radiusSquared = radius * radius;
    for (size_t i = 0; i < _points.size(); ++i) {
        Vector3D r = _points[i] - origin;
        double distanceSquared = r.dot(r);
        if (distanceSquared <= radiusSquared) {
            callback(i, _points[i]);
        }
    }
}

bool PointSimpleListSearcher3::hasNearbyPoint(const Point3D& origin, double radius) const {
    double radiusSquared = radius * radius;
    for (const auto& _point : _points) {
        Vector3D r = _point - origin;
        double distanceSquared = r.dot(r);
        if (distanceSquared <= radiusSquared) {
            return true;
        }
    }

    return false;
}

PointNeighborSearcher3Ptr PointSimpleListSearcher3::clone() const {
        return CLONE_W_CUSTOM_DELETER(PointSimpleListSearcher3)}

PointSimpleListSearcher3& PointSimpleListSearcher3::operator=(const PointSimpleListSearcher3& other) {
    set(other);
    return *this;
}

void PointSimpleListSearcher3::set(const PointSimpleListSearcher3& other) { _points = other._points; }

PointSimpleListSearcher3 PointSimpleListSearcher3::Builder::build() const { return {}; }

PointSimpleListSearcher3Ptr PointSimpleListSearcher3::Builder::makeShared() const {
    return {new PointSimpleListSearcher3(), [](PointSimpleListSearcher3* obj) { delete obj; }};
}

PointNeighborSearcher3Ptr PointSimpleListSearcher3::Builder::buildPointNeighborSearcher() const { return makeShared(); }
