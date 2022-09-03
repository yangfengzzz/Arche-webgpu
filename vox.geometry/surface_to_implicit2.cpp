// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/surface_to_implicit2.h"

#include <utility>

using namespace vox;

SurfaceToImplicit2::SurfaceToImplicit2(Surface2Ptr surface, const Transform2D &transform, bool isNormalFlipped)
    : ImplicitSurface2(transform, isNormalFlipped), _surface(std::move(surface)) {}

SurfaceToImplicit2::SurfaceToImplicit2(const SurfaceToImplicit2 &other) = default;

bool SurfaceToImplicit2::isBounded() const { return _surface->isBounded(); }

void SurfaceToImplicit2::updateQueryEngine() { _surface->updateQueryEngine(); }

bool SurfaceToImplicit2::isValidGeometry() const { return _surface->isValidGeometry(); }

Surface2Ptr SurfaceToImplicit2::surface() const { return _surface; }

SurfaceToImplicit2::Builder SurfaceToImplicit2::builder() { return {}; }

Point2D SurfaceToImplicit2::closestPointLocal(const Point2D &otherPoint) const {
    return _surface->closestPoint(otherPoint);
}

Vector2D SurfaceToImplicit2::closestNormalLocal(const Point2D &otherPoint) const {
    return _surface->closestNormal(otherPoint);
}

double SurfaceToImplicit2::closestDistanceLocal(const Point2D &otherPoint) const {
    return _surface->closestDistance(otherPoint);
}

bool SurfaceToImplicit2::intersectsLocal(const Ray2D &ray) const { return _surface->intersects(ray); }

SurfaceRayIntersection2 SurfaceToImplicit2::closestIntersectionLocal(const Ray2D &ray) const {
    return _surface->closestIntersection(ray);
}

BoundingBox2D SurfaceToImplicit2::boundingBoxLocal() const { return _surface->boundingBox(); }

bool SurfaceToImplicit2::isInsideLocal(const Point2D &otherPoint) const { return _surface->isInside(otherPoint); }

double SurfaceToImplicit2::signedDistanceLocal(const Point2D &otherPoint) const {
    Point2D x = _surface->closestPoint(otherPoint);
    bool inside = _surface->isInside(otherPoint);
    return (inside) ? -x.distanceTo(otherPoint) : x.distanceTo(otherPoint);
}

SurfaceToImplicit2::Builder &SurfaceToImplicit2::Builder::withSurface(const Surface2Ptr &surface) {
    _surface = surface;
    return *this;
}

SurfaceToImplicit2 SurfaceToImplicit2::Builder::build() const {
    return SurfaceToImplicit2(_surface, _transform, _isNormalFlipped);
}

SurfaceToImplicit2Ptr SurfaceToImplicit2::Builder::makeShared() const {
    return {new SurfaceToImplicit2(_surface, _transform, _isNormalFlipped),
            [](SurfaceToImplicit2 *obj) { delete obj; }};
}
