// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/plane2.h"

using namespace vox;

Plane2::Plane2(const Transform2D& transform_, bool isNormalFlipped_) : Surface2(transform_, isNormalFlipped_) {}

Plane2::Plane2(const Vector2D& normal_, const Point2D& point_, const Transform2D& transform_, bool isNormalFlipped_)
    : Surface2(transform_, isNormalFlipped_), normal(normal_), point(point_) {}

Plane2::Plane2(const Plane2& other) = default;

bool Plane2::isBounded() const { return false; }

Point2D Plane2::closestPointLocal(const Point2D& otherPoint) const {
    Vector2D r = otherPoint - point;
    return point + (r - normal.dot(r) * normal);
}

Vector2D Plane2::closestNormalLocal(const Point2D& otherPoint) const { return normal; }

bool Plane2::intersectsLocal(const Ray2D& ray) const { return std::fabs(ray.direction.dot(normal)) > 0; }

SurfaceRayIntersection2 Plane2::closestIntersectionLocal(const Ray2D& ray) const {
    SurfaceRayIntersection2 intersection;
    double dDotN = ray.direction.dot(normal);

    // Check if not parallel
    if (std::fabs(dDotN) > 0) {
        double t = normal.dot(point - ray.origin) / dDotN;
        if (t >= 0.0) {
            intersection.isIntersecting = true;
            intersection.distance = t;
            intersection.point = ray.pointAt(t);
            intersection.normal = normal;
        }
    }

    return intersection;
}

BoundingBox2D Plane2::boundingBoxLocal() const {
    if (std::fabs(normal.dot(Vector2D(1, 0)) - 1.0) < kEpsilonD) {
        return BoundingBox2D(point - Vector2D(0, kMaxD), point + Vector2D(0, kMaxD));
    } else if (std::fabs(normal.dot(Vector2D(0, 1)) - 1.0) < kEpsilonD) {
        return BoundingBox2D(point - Vector2D(kMaxD, 0), point + Vector2D(kMaxD, 0));
    } else {
        return BoundingBox2D(Point2D(kMaxD, kMaxD), Point2D(kMaxD, kMaxD));
    }
}

Plane2::Builder Plane2::builder() { return Builder(); }

Plane2::Builder& Plane2::Builder::withNormal(const Vector2D& normal) {
    _normal = normal;
    return *this;
}

Plane2::Builder& Plane2::Builder::withPoint(const Point2D& point) {
    _point = point;
    return *this;
}

Plane2 Plane2::Builder::build() const { return Plane2(_normal, _point, _transform, _isNormalFlipped); }

Plane2Ptr Plane2::Builder::makeShared() const {
    return {new Plane2(_normal, _point, _transform, _isNormalFlipped), [](Plane2* obj) { delete obj; }};
}
