// Copyright (c) Doyub Kim
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/implicit_surface_set2.h"

#include <utility>

#include "vox.geometry/surface_to_implicit2.h"

using namespace vox;

ImplicitSurfaceSet2::ImplicitSurfaceSet2() = default;

ImplicitSurfaceSet2::ImplicitSurfaceSet2(std::vector<ImplicitSurface2Ptr> surfaces,
                                         const Transform2D &transform,
                                         bool isNormalFlipped)
    : ImplicitSurface2(transform, isNormalFlipped), _surfaces(std::move(surfaces)) {
    for (const auto &surface : _surfaces) {
        if (!surface->isBounded()) {
            _unboundedSurfaces.push_back(surface);
        }
    }
    invalidateBvh();
}

ImplicitSurfaceSet2::ImplicitSurfaceSet2(const std::vector<Surface2Ptr> &surfaces,
                                         const Transform2D &transform,
                                         bool isNormalFlipped)
    : ImplicitSurface2(transform, isNormalFlipped) {
    for (const auto &surface : surfaces) {
        addExplicitSurface(surface);
    }
}

ImplicitSurfaceSet2::ImplicitSurfaceSet2(const ImplicitSurfaceSet2 &other)
    : ImplicitSurface2(other), _surfaces(other._surfaces), _unboundedSurfaces(other._unboundedSurfaces) {}

void ImplicitSurfaceSet2::updateQueryEngine() {
    invalidateBvh();
    buildBvh();
}

bool ImplicitSurfaceSet2::isBounded() const {
    // All surfaces should be bounded.
    for (const auto &surface : _surfaces) {
        if (!surface->isBounded()) {
            return false;
        }
    }

    // Empty set is not bounded.
    return !_surfaces.empty();
}

bool ImplicitSurfaceSet2::isValidGeometry() const {
    // All surfaces should be valid.
    for (const auto &surface : _surfaces) {
        if (!surface->isValidGeometry()) {
            return false;
        }
    }

    // Empty set is not valid.
    return !_surfaces.empty();
}

size_t ImplicitSurfaceSet2::numberOfSurfaces() const { return _surfaces.size(); }

const ImplicitSurface2Ptr &ImplicitSurfaceSet2::surfaceAt(size_t i) const { return _surfaces[i]; }

void ImplicitSurfaceSet2::addExplicitSurface(const Surface2Ptr &surface) {
    addSurface(std::make_shared<SurfaceToImplicit2>(surface));
}

void ImplicitSurfaceSet2::addSurface(const ImplicitSurface2Ptr &surface) {
    _surfaces.push_back(surface);
    if (!surface->isBounded()) {
        _unboundedSurfaces.push_back(surface);
    }
    invalidateBvh();
}

Point2D ImplicitSurfaceSet2::closestPointLocal(const Point2D &otherPoint) const {
    buildBvh();

    const auto distanceFunc = [](const Surface2Ptr &surface, const Point2D &pt) {
        return surface->closestDistance(pt);
    };

    Point2D result{kMaxD, kMaxD};
    const auto queryResult = _bvh.nearest(otherPoint, distanceFunc);
    if (queryResult.item != nullptr) {
        result = (*queryResult.item)->closestPoint(otherPoint);
    }

    double minDist = queryResult.distance;
    for (const auto &surface : _unboundedSurfaces) {
        auto pt = surface->closestPoint(otherPoint);
        double dist = pt.distanceTo(otherPoint);
        if (dist < minDist) {
            minDist = dist;
            result = surface->closestPoint(otherPoint);
        }
    }

    return result;
}

double ImplicitSurfaceSet2::closestDistanceLocal(const Point2D &otherPoint) const {
    buildBvh();

    const auto distanceFunc = [](const Surface2Ptr &surface, const Point2D &pt) {
        return surface->closestDistance(pt);
    };

    const auto queryResult = _bvh.nearest(otherPoint, distanceFunc);

    double minDist = queryResult.distance;
    for (const auto &surface : _unboundedSurfaces) {
        auto pt = surface->closestPoint(otherPoint);
        double dist = pt.distanceTo(otherPoint);
        if (dist < minDist) {
            minDist = dist;
        }
    }

    return minDist;
}

Vector2D ImplicitSurfaceSet2::closestNormalLocal(const Point2D &otherPoint) const {
    buildBvh();

    const auto distanceFunc = [](const Surface2Ptr &surface, const Point2D &pt) {
        return surface->closestDistance(pt);
    };

    Vector2D result{1.0, 0.0};
    const auto queryResult = _bvh.nearest(otherPoint, distanceFunc);
    if (queryResult.item != nullptr) {
        result = (*queryResult.item)->closestNormal(otherPoint);
    }

    double minDist = queryResult.distance;
    for (const auto &surface : _unboundedSurfaces) {
        auto pt = surface->closestPoint(otherPoint);
        double dist = pt.distanceTo(otherPoint);
        if (dist < minDist) {
            minDist = dist;
            result = surface->closestNormal(otherPoint);
        }
    }

    return result;
}

bool ImplicitSurfaceSet2::intersectsLocal(const Ray2D &ray) const {
    buildBvh();

    const auto testFunc = [](const Surface2Ptr &surface, const Ray2D &ray) { return surface->intersects(ray); };

    bool result = _bvh.intersects(ray, testFunc);
    for (const auto &surface : _unboundedSurfaces) {
        result |= surface->intersects(ray);
    }

    return result;
}

SurfaceRayIntersection2 ImplicitSurfaceSet2::closestIntersectionLocal(const Ray2D &ray) const {
    buildBvh();

    const auto testFunc = [](const Surface2Ptr &surface, const Ray2D &ray) {
        SurfaceRayIntersection2 result = surface->closestIntersection(ray);
        return result.distance;
    };

    const auto queryResult = _bvh.closestIntersection(ray, testFunc);
    SurfaceRayIntersection2 result;
    result.distance = queryResult.distance;
    result.isIntersecting = queryResult.item != nullptr;
    if (queryResult.item != nullptr) {
        result.point = ray.pointAt(queryResult.distance);
        result.normal = (*queryResult.item)->closestNormal(result.point);
    }

    for (const auto &surface : _unboundedSurfaces) {
        SurfaceRayIntersection2 localResult = surface->closestIntersection(ray);
        if (localResult.distance < result.distance) {
            result = localResult;
        }
    }

    return result;
}

BoundingBox2D ImplicitSurfaceSet2::boundingBoxLocal() const {
    buildBvh();

    return _bvh.boundingBox();
}

bool ImplicitSurfaceSet2::isInsideLocal(const Point2D &otherPoint) const {
    for (const auto &surface : _surfaces) {
        if (surface->isInside(otherPoint)) {
            return true;
        }
    }

    return false;
}

double ImplicitSurfaceSet2::signedDistanceLocal(const Point2D &otherPoint) const {
    double sdf = kMaxD;
    for (const auto &surface : _surfaces) {
        sdf = std::min(sdf, surface->signedDistance(otherPoint));
    }

    return sdf;
}

void ImplicitSurfaceSet2::invalidateBvh() { _bvhInvalidated = true; }

void ImplicitSurfaceSet2::buildBvh() const {
    if (_bvhInvalidated) {
        std::vector<ImplicitSurface2Ptr> surfs;
        std::vector<BoundingBox2D> bounds;
        for (const auto &_surface : _surfaces) {
            if (_surface->isBounded()) {
                surfs.push_back(_surface);
                bounds.push_back(_surface->boundingBox());
            }
        }
        _bvh.build(surfs, bounds);
        _bvhInvalidated = false;
    }
}

// ImplicitSurfaceSet2::Builder

ImplicitSurfaceSet2::Builder ImplicitSurfaceSet2::builder() { return {}; }

ImplicitSurfaceSet2::Builder &ImplicitSurfaceSet2::Builder::withSurfaces(
        const std::vector<ImplicitSurface2Ptr> &surfaces) {
    _surfaces = surfaces;
    return *this;
}

ImplicitSurfaceSet2::Builder &ImplicitSurfaceSet2::Builder::withExplicitSurfaces(
        const std::vector<Surface2Ptr> &surfaces) {
    _surfaces.clear();
    for (const auto &surface : surfaces) {
        _surfaces.push_back(std::make_shared<SurfaceToImplicit2>(surface));
    }
    return *this;
}

ImplicitSurfaceSet2 ImplicitSurfaceSet2::Builder::build() const {
    return ImplicitSurfaceSet2(_surfaces, _transform, _isNormalFlipped);
}

ImplicitSurfaceSet2Ptr ImplicitSurfaceSet2::Builder::makeShared() const {
    return {new ImplicitSurfaceSet2(_surfaces, _transform, _isNormalFlipped),
            [](ImplicitSurfaceSet2 *obj) { delete obj; }};
}
