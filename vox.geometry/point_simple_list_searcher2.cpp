// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/point_simple_list_searcher2.h"

#include <algorithm>

#include "vox.base/private_helpers.h"

using namespace vox;

PointSimpleListSearcher2::PointSimpleListSearcher2() = default;

PointSimpleListSearcher2::PointSimpleListSearcher2(const PointSimpleListSearcher2& other) { set(other); }

void PointSimpleListSearcher2::build(const ConstArrayAccessor1<Point2D>& points) {
    _points.resize(points.size());
    std::copy(points.data(), points.data() + points.size(), _points.begin());
}

void PointSimpleListSearcher2::forEachNearbyPoint(const Point2D& origin,
                                                  double radius,
                                                  const ForEachNearbyPointFunc& callback) const {
    double radiusSquared = radius * radius;
    for (size_t i = 0; i < _points.size(); ++i) {
        Vector2D r = _points[i] - origin;
        double distanceSquared = r.dot(r);
        if (distanceSquared <= radiusSquared) {
            callback(i, _points[i]);
        }
    }
}

bool PointSimpleListSearcher2::hasNearbyPoint(const Point2D& origin, double radius) const {
    double radiusSquared = radius * radius;
    for (const auto& _point : _points) {
        Vector2D r = _point - origin;
        double distanceSquared = r.dot(r);
        if (distanceSquared <= radiusSquared) {
            return true;
        }
    }

    return false;
}

PointNeighborSearcher2Ptr PointSimpleListSearcher2::clone() const {
        return CLONE_W_CUSTOM_DELETER(PointSimpleListSearcher2)}

PointSimpleListSearcher2& PointSimpleListSearcher2::operator=(const PointSimpleListSearcher2& other) {
    set(other);
    return *this;
}

void PointSimpleListSearcher2::set(const PointSimpleListSearcher2& other) { _points = other._points; }

PointSimpleListSearcher2 PointSimpleListSearcher2::Builder::build() const { return {}; }

PointSimpleListSearcher2Ptr PointSimpleListSearcher2::Builder::makeShared() const {
    return {new PointSimpleListSearcher2(), [](PointSimpleListSearcher2* obj) { delete obj; }};
}

PointNeighborSearcher2Ptr PointSimpleListSearcher2::Builder::buildPointNeighborSearcher() const { return makeShared(); }
