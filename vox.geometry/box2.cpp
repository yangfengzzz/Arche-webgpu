// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/box2.h"

#include "vox.geometry/plane2.h"

using namespace vox;

Box2::Box2(const Transform2D& transform, bool isNormalFlipped) : Surface2(transform, isNormalFlipped) {}

Box2::Box2(const Point2D& lowerCorner, const Point2D& upperCorner, const Transform2D& transform, bool isNormalFlipped)
    : Box2(BoundingBox2D(lowerCorner, upperCorner), transform, isNormalFlipped) {}

Box2::Box2(const BoundingBox2D& boundingBox, const Transform2D& transform, bool isNormalFlipped)
    : Surface2(transform, isNormalFlipped), bound(boundingBox) {}

Box2::Box2(const Box2& other) = default;

Point2D Box2::closestPointLocal(const Point2D& otherPoint) const {
    if (bound.contains(otherPoint)) {
        Plane2 planes[4] = {Plane2(Vector2D(1, 0), bound.upper_corner), Plane2(Vector2D(0, 1), bound.upper_corner),
                            Plane2(Vector2D(-1, 0), bound.lower_corner), Plane2(Vector2D(0, -1), bound.lower_corner)};

        Point2D result = planes[0].closestPoint(otherPoint);
        double distanceSquared = result.distanceSquaredTo(otherPoint);

        for (int i = 1; i < 4; ++i) {
            Point2D localResult = planes[i].closestPoint(otherPoint);
            double localDistanceSquared = localResult.distanceSquaredTo(otherPoint);

            if (localDistanceSquared < distanceSquared) {
                result = localResult;
                distanceSquared = localDistanceSquared;
            }
        }

        return result;
    } else {
        return clamp(otherPoint, bound.lower_corner, bound.upper_corner);
    }
}

Vector2D Box2::closestNormalLocal(const Point2D& otherPoint) const {
    Plane2 planes[4] = {Plane2(Vector2D(1, 0), bound.upper_corner), Plane2(Vector2D(0, 1), bound.upper_corner),
                        Plane2(Vector2D(-1, 0), bound.lower_corner), Plane2(Vector2D(0, -1), bound.lower_corner)};

    if (bound.contains(otherPoint)) {
        Vector2D closestNormal = planes[0].normal;
        Point2D closestPoint = planes[0].closestPoint(otherPoint);
        double minDistanceSquared = (closestPoint - otherPoint).lengthSquared();

        for (int i = 1; i < 4; ++i) {
            Point2D localClosestPoint = planes[i].closestPoint(otherPoint);
            double localDistanceSquared = (localClosestPoint - otherPoint).lengthSquared();

            if (localDistanceSquared < minDistanceSquared) {
                closestNormal = planes[i].normal;
                minDistanceSquared = localDistanceSquared;
            }
        }

        return closestNormal;
    } else {
        Point2D closestPoint = clamp(otherPoint, bound.lower_corner, bound.upper_corner);
        Vector2D closestPointToInputPoint = otherPoint - closestPoint;
        Vector2D closestNormal = planes[0].normal;
        double maxCosineAngle = closestNormal.dot(closestPointToInputPoint);

        for (int i = 1; i < 4; ++i) {
            double cosineAngle = planes[i].normal.dot(closestPointToInputPoint);

            if (cosineAngle > maxCosineAngle) {
                closestNormal = planes[i].normal;
                maxCosineAngle = cosineAngle;
            }
        }

        return closestNormal;
    }
}

bool Box2::intersectsLocal(const Ray2D& ray) const { return bound.intersects(ray); }

SurfaceRayIntersection2 Box2::closestIntersectionLocal(const Ray2D& ray) const {
    SurfaceRayIntersection2 intersection;
    BoundingBoxRayIntersection2D bbRayIntersection = bound.closestIntersection(ray);
    intersection.isIntersecting = bbRayIntersection.is_intersecting;
    if (intersection.isIntersecting) {
        intersection.distance = bbRayIntersection.t_near;
        intersection.point = ray.pointAt(bbRayIntersection.t_near);
        intersection.normal = closestNormalLocal(intersection.point);
    }
    return intersection;
}

BoundingBox2D Box2::boundingBoxLocal() const { return bound; }

Box2::Builder Box2::builder() { return {}; }

Box2::Builder& Box2::Builder::withLowerCorner(const Point2D& pt) {
    _lowerCorner = pt;
    return *this;
}

Box2::Builder& Box2::Builder::withUpperCorner(const Point2D& pt) {
    _upperCorner = pt;
    return *this;
}

Box2::Builder& Box2::Builder::withBoundingBox(const BoundingBox2D& bbox) {
    _lowerCorner = bbox.lower_corner;
    _upperCorner = bbox.upper_corner;
    return *this;
}

Box2 Box2::Builder::build() const { return Box2(_lowerCorner, _upperCorner, _transform, _isNormalFlipped); }

Box2Ptr Box2::Builder::makeShared() const {
    return {new Box2(_lowerCorner, _upperCorner, _transform, _isNormalFlipped), [](Box2* obj) { delete obj; }};
}
