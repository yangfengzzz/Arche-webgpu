//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>

#include "vox.base/constants.h"
#include "vox.math/bounding_box3.h"

namespace vox {

//! Closest intersection query result.
template <typename T>
struct ClosestIntersectionQueryResult3 {
    const T *item = nullptr;
    double distance = kMaxD;
};

//! Closest intersection distance measure function.
template <typename T>
using ClosestIntersectionDistanceFunc3 = std::function<double(const T &, const Vector3D &)>;

//! Box-item intersection test function.
template <typename T>
using BoxIntersectionTestFunc3 = std::function<bool(const T &, const BoundingBox3D &)>;

//! Ray-item intersection test function.
template <typename T>
using RayIntersectionTestFunc3 = std::function<bool(const T &, const Ray3D &)>;

//! Ray-item closest intersection evaluation function.
template <typename T>
using GetRayIntersectionFunc3 = std::function<double(const T &, const Ray3D &)>;

//! Visitor function which is invoked for each intersecting item.
template <typename T>
using IntersectionVisitorFunc3 = std::function<void(const T &)>;

//! Abstract base class for 3-D intersection test query engine.
template <typename T>
class IntersectionQueryEngine3 {
public:
    //! Returns true if given \p box intersects with any of the stored items.
    virtual bool intersects(const BoundingBox3D &box, const BoxIntersectionTestFunc3<T> &testFunc) const = 0;

    //! Returns true if given \p ray intersects with any of the stored items.
    virtual bool intersects(const Ray3D &ray, const RayIntersectionTestFunc3<T> &testFunc) const = 0;

    //! Invokes \p visitorFunc for every intersecting items.
    virtual void forEachIntersectingItem(const BoundingBox3D &box,
                                         const BoxIntersectionTestFunc3<T> &testFunc,
                                         const IntersectionVisitorFunc3<T> &visitorFunc) const = 0;

    //! Invokes \p visitorFunc for every intersecting items.
    virtual void forEachIntersectingItem(const Ray3D &ray,
                                         const RayIntersectionTestFunc3<T> &testFunc,
                                         const IntersectionVisitorFunc3<T> &visitorFunc) const = 0;

    //! Returns the closest intersection for given \p ray.
    virtual ClosestIntersectionQueryResult3<T> closestIntersection(
            const Ray3D &ray, const GetRayIntersectionFunc3<T> &testFunc) const = 0;
};

}  // namespace vox
