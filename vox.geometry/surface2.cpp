// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/surface2.h"

#include <utility>

using namespace vox;

Surface2::Surface2(Transform2D transform_, bool isNormalFlipped_)
    : transform(std::move(transform_)), isNormalFlipped(isNormalFlipped_) {}

Surface2::Surface2(const Surface2 &other) = default;

Surface2::~Surface2() = default;

Point2D Surface2::closestPoint(const Point2D &otherPoint) const {
    return transform.toWorld(closestPointLocal(transform.toLocal(otherPoint)));
}

BoundingBox2D Surface2::boundingBox() const { return transform.toWorld(boundingBoxLocal()); }

bool Surface2::intersects(const Ray2D &ray) const { return intersectsLocal(transform.toLocal(ray)); }

double Surface2::closestDistance(const Point2D &otherPoint) const {
    return closestDistanceLocal(transform.toLocal(otherPoint));
}

SurfaceRayIntersection2 Surface2::closestIntersection(const Ray2D &ray) const {
    auto result = closestIntersectionLocal(transform.toLocal(ray));
    result.point = transform.toWorld(result.point);
    result.normal = transform.toWorldDirection(result.normal);
    result.normal *= (isNormalFlipped) ? -1.0 : 1.0;
    return result;
}

Vector2D Surface2::closestNormal(const Point2D &otherPoint) const {
    auto result = transform.toWorldDirection(closestNormalLocal(transform.toLocal(otherPoint)));
    result *= (isNormalFlipped) ? -1.0 : 1.0;
    return result;
}

void Surface2::updateQueryEngine() {
    // Do nothing
}

bool Surface2::isBounded() const { return true; }

bool Surface2::isValidGeometry() const { return true; }

bool Surface2::isInside(const Point2D &otherPoint) const {
    return isNormalFlipped == !isInsideLocal(transform.toLocal(otherPoint));
}

bool Surface2::intersectsLocal(const Ray2D &rayLocal) const {
    auto result = closestIntersectionLocal(rayLocal);
    return result.isIntersecting;
}

double Surface2::closestDistanceLocal(const Point2D &otherPointLocal) const {
    return otherPointLocal.distanceTo(closestPointLocal(otherPointLocal));
}

bool Surface2::isInsideLocal(const Point2D &otherPointLocal) const {
    Point2D cpLocal = closestPointLocal(otherPointLocal);
    Vector2D normalLocal = closestNormalLocal(otherPointLocal);
    return (otherPointLocal - cpLocal).dot(normalLocal) < 0.0;
}
