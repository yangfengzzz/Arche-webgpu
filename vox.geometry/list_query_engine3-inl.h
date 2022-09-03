//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.geometry/list_query_engine3.h"

namespace vox {

template <typename T>
void ListQueryEngine3<T>::add(const T& item) {
    _items.push_back(item);
}

template <typename T>
void ListQueryEngine3<T>::add(const std::vector<T>& items) {
    _items.insert(_items.end(), items.begin(), items.end());
}

template <typename T>
bool ListQueryEngine3<T>::intersects(const BoundingBox3D& box, const BoxIntersectionTestFunc3<T>& testFunc) const {
    for (const auto& item : _items) {
        if (testFunc(item, box)) {
            return true;
        }
    }

    return false;
}

template <typename T>
bool ListQueryEngine3<T>::intersects(const Ray3D& ray, const RayIntersectionTestFunc3<T>& testFunc) const {
    for (const auto& item : _items) {
        if (testFunc(item, ray)) {
            return true;
        }
    }

    return false;
}

template <typename T>
void ListQueryEngine3<T>::forEachIntersectingItem(const BoundingBox3D& box,
                                                  const BoxIntersectionTestFunc3<T>& testFunc,
                                                  const IntersectionVisitorFunc3<T>& visitorFunc) const {
    for (const auto& item : _items) {
        if (testFunc(item, box)) {
            visitorFunc(item);
        }
    }
}

template <typename T>
void ListQueryEngine3<T>::forEachIntersectingItem(const Ray3D& ray,
                                                  const RayIntersectionTestFunc3<T>& testFunc,
                                                  const IntersectionVisitorFunc3<T>& visitorFunc) const {
    for (const auto& item : _items) {
        if (testFunc(item, ray)) {
            visitorFunc(item);
        }
    }
}

template <typename T>
ClosestIntersectionQueryResult3<T> ListQueryEngine3<T>::closestIntersection(
        const Ray3D& ray, const GetRayIntersectionFunc3<T>& testFunc) const {
    ClosestIntersectionQueryResult3<T> best;
    for (const auto& item : _items) {
        double dist = testFunc(item, ray);
        if (dist < best.distance) {
            best.distance = dist;
            best.item = &item;
        }
    }

    return best;
}

template <typename T>
NearestNeighborQueryResult3<T> ListQueryEngine3<T>::nearest(const Point3D& pt,
                                                            const NearestNeighborDistanceFunc3<T>& distanceFunc) const {
    NearestNeighborQueryResult3<T> best;
    for (const auto& item : _items) {
        double dist = distanceFunc(item, pt);
        if (dist < best.distance) {
            best.item = &item;
            best.distance = dist;
        }
    }

    return best;
}

}  // namespace vox