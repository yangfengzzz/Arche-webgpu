// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/box3.h"

#include "vox.geometry/plane3.h"

using namespace vox;

Box3::Box3(const Transform3D& transform, bool isNormalFlipped) : Surface3(transform, isNormalFlipped) {}

Box3::Box3(const Point3D& lowerCorner, const Point3D& upperCorner, const Transform3D& transform, bool isNormalFlipped)
    : Box3(BoundingBox3D(lowerCorner, upperCorner), transform, isNormalFlipped) {}

Box3::Box3(const BoundingBox3D& boundingBox, const Transform3D& transform, bool isNormalFlipped)
    : Surface3(transform, isNormalFlipped), bound(boundingBox) {}

Box3::Box3(const Box3& other) = default;

Point3D Box3::closestPointLocal(const Point3D& otherPoint) const {
    if (bound.contains(otherPoint)) {
        Plane3 planes[6] = {
                Plane3(Vector3D(1, 0, 0), bound.upper_corner),  Plane3(Vector3D(0, 1, 0), bound.upper_corner),
                Plane3(Vector3D(0, 0, 1), bound.upper_corner),  Plane3(Vector3D(-1, 0, 0), bound.lower_corner),
                Plane3(Vector3D(0, -1, 0), bound.lower_corner), Plane3(Vector3D(0, 0, -1), bound.lower_corner)};

        Point3D result = planes[0].closestPoint(otherPoint);
        double distanceSquared = result.distanceSquaredTo(otherPoint);

        for (int i = 1; i < 6; ++i) {
            Point3D localResult = planes[i].closestPoint(otherPoint);
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

Vector3D Box3::closestNormalLocal(const Point3D& otherPoint) const {
    Plane3 planes[6] = {Plane3(Vector3D(1, 0, 0), bound.upper_corner),  Plane3(Vector3D(0, 1, 0), bound.upper_corner),
                        Plane3(Vector3D(0, 0, 1), bound.upper_corner),  Plane3(Vector3D(-1, 0, 0), bound.lower_corner),
                        Plane3(Vector3D(0, -1, 0), bound.lower_corner), Plane3(Vector3D(0, 0, -1), bound.lower_corner)};

    if (bound.contains(otherPoint)) {
        Vector3D closestNormal = planes[0].normal;
        Point3D closestPoint = planes[0].closestPoint(otherPoint);
        double minDistanceSquared = (closestPoint - otherPoint).lengthSquared();

        for (int i = 1; i < 6; ++i) {
            Point3D localClosestPoint = planes[i].closestPoint(otherPoint);
            double localDistanceSquared = (localClosestPoint - otherPoint).lengthSquared();

            if (localDistanceSquared < minDistanceSquared) {
                closestNormal = planes[i].normal;
                minDistanceSquared = localDistanceSquared;
            }
        }

        return closestNormal;
    } else {
        Point3D closestPoint = clamp(otherPoint, bound.lower_corner, bound.upper_corner);
        Vector3D closestPointToInputPoint = otherPoint - closestPoint;
        Vector3D closestNormal = planes[0].normal;
        double maxCosineAngle = closestNormal.dot(closestPointToInputPoint);

        for (int i = 1; i < 6; ++i) {
            double cosineAngle = planes[i].normal.dot(closestPointToInputPoint);

            if (cosineAngle > maxCosineAngle) {
                closestNormal = planes[i].normal;
                maxCosineAngle = cosineAngle;
            }
        }

        return closestNormal;
    }
}

bool Box3::intersectsLocal(const Ray3D& ray) const { return bound.intersects(ray); }

SurfaceRayIntersection3 Box3::closestIntersectionLocal(const Ray3D& ray) const {
    SurfaceRayIntersection3 intersection;
    BoundingBoxRayIntersection3D bbRayIntersection = bound.closestIntersection(ray);
    intersection.isIntersecting = bbRayIntersection.is_intersecting;
    if (intersection.isIntersecting) {
        intersection.distance = bbRayIntersection.t_near;
        intersection.point = ray.pointAt(bbRayIntersection.t_near);
        intersection.normal = closestNormalLocal(intersection.point);
    }

    return intersection;
}

BoundingBox3D Box3::boundingBoxLocal() const { return bound; }

Box3::Builder Box3::builder() { return {}; }

Box3::Builder& Box3::Builder::withLowerCorner(const Point3D& pt) {
    _lowerCorner = pt;
    return *this;
}

Box3::Builder& Box3::Builder::withUpperCorner(const Point3D& pt) {
    _upperCorner = pt;
    return *this;
}

Box3::Builder& Box3::Builder::withBoundingBox(const BoundingBox3D& bbox) {
    _lowerCorner = bbox.lower_corner;
    _upperCorner = bbox.upper_corner;
    return *this;
}

Box3 Box3::Builder::build() const { return Box3(_lowerCorner, _upperCorner, _transform, _isNormalFlipped); }

Box3Ptr Box3::Builder::makeShared() const {
    return {new Box3(_lowerCorner, _upperCorner, _transform, _isNormalFlipped), [](Box3* obj) { delete obj; }};
}
