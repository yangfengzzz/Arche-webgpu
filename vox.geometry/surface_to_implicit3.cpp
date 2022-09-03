// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/surface_to_implicit3.h"

#include "vox.base/logging.h"
#include "vox.geometry/triangle_mesh3.h"

using namespace vox;

SurfaceToImplicit3::SurfaceToImplicit3(const Surface3Ptr &surface, const Transform3D &transform, bool isNormalFlipped)
    : ImplicitSurface3(transform, isNormalFlipped), _surface(surface) {
    if (std::dynamic_pointer_cast<TriangleMesh3>(surface) != nullptr) {
        LOGW("Using TriangleMesh3 with SurfaceToImplicit3 is accurate "
             "but slow. ImplicitTriangleMesh3 can provide faster but "
             "approximated results.")
    }
}

SurfaceToImplicit3::SurfaceToImplicit3(const SurfaceToImplicit3 &other) = default;

bool SurfaceToImplicit3::isBounded() const { return _surface->isBounded(); }

void SurfaceToImplicit3::updateQueryEngine() { _surface->updateQueryEngine(); }

bool SurfaceToImplicit3::isValidGeometry() const { return _surface->isValidGeometry(); }

Surface3Ptr SurfaceToImplicit3::surface() const { return _surface; }

SurfaceToImplicit3::Builder SurfaceToImplicit3::builder() { return {}; }

Point3D SurfaceToImplicit3::closestPointLocal(const Point3D &otherPoint) const {
    return _surface->closestPoint(otherPoint);
}

Vector3D SurfaceToImplicit3::closestNormalLocal(const Point3D &otherPoint) const {
    return _surface->closestNormal(otherPoint);
}

double SurfaceToImplicit3::closestDistanceLocal(const Point3D &otherPoint) const {
    return _surface->closestDistance(otherPoint);
}

bool SurfaceToImplicit3::intersectsLocal(const Ray3D &ray) const { return _surface->intersects(ray); }

SurfaceRayIntersection3 SurfaceToImplicit3::closestIntersectionLocal(const Ray3D &ray) const {
    return _surface->closestIntersection(ray);
}

BoundingBox3D SurfaceToImplicit3::boundingBoxLocal() const { return _surface->boundingBox(); }

double SurfaceToImplicit3::signedDistanceLocal(const Point3D &otherPoint) const {
    Point3D x = _surface->closestPoint(otherPoint);
    bool inside = _surface->isInside(otherPoint);
    return (inside) ? -x.distanceTo(otherPoint) : x.distanceTo(otherPoint);
}

bool SurfaceToImplicit3::isInsideLocal(const Point3D &otherPoint) const { return _surface->isInside(otherPoint); }

SurfaceToImplicit3::Builder &SurfaceToImplicit3::Builder::withSurface(const Surface3Ptr &surface) {
    _surface = surface;
    return *this;
}

SurfaceToImplicit3 SurfaceToImplicit3::Builder::build() const {
    return SurfaceToImplicit3(_surface, _transform, _isNormalFlipped);
}

SurfaceToImplicit3Ptr SurfaceToImplicit3::Builder::makeShared() const {
    return {new SurfaceToImplicit3(_surface, _transform, _isNormalFlipped),
            [](SurfaceToImplicit3 *obj) { delete obj; }};
}
