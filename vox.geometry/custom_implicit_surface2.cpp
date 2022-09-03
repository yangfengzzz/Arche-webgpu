// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/custom_implicit_surface2.h"

#include <utility>

#include "vox.base/constants.h"

using namespace vox;

CustomImplicitSurface2::CustomImplicitSurface2(std::function<double(const Point2D &)> func,
                                               const BoundingBox2D &domain,
                                               double resolution,
                                               double rayMarchingResolution,
                                               unsigned int maxNumOfIterations,
                                               const Transform2D &transform,
                                               bool isNormalFlipped)
    : ImplicitSurface2(transform, isNormalFlipped),
      _func(std::move(func)),
      _domain(domain),
      _resolution(resolution),
      _rayMarchingResolution(rayMarchingResolution),
      _maxNumOfIterations(maxNumOfIterations) {}

CustomImplicitSurface2::~CustomImplicitSurface2() = default;

Point2D CustomImplicitSurface2::closestPointLocal(const Point2D &otherPoint) const {
    Point2D pt = clamp(otherPoint, _domain.lower_corner, _domain.upper_corner);
    for (unsigned int iter = 0; iter < _maxNumOfIterations; ++iter) {
        double sdf = signedDistanceLocal(pt);
        if (std::fabs(sdf) < kEpsilonD) {
            break;
        }
        Vector2D g = gradientLocal(pt);
        pt = pt - sdf * g;
    }
    return pt;
}

bool CustomImplicitSurface2::intersectsLocal(const Ray2D &ray) const {
    BoundingBoxRayIntersection2D intersection = _domain.closestIntersection(ray);

    if (intersection.is_intersecting) {
        double tStart, tEnd;
        if (intersection.t_far == kMaxD) {
            tStart = 0.0;
            tEnd = intersection.t_near;
        } else {
            tStart = intersection.t_near;
            tEnd = intersection.t_far;
        }

        double t = tStart;
        Point2D pt = ray.pointAt(t);
        double prevPhi = _func(pt);
        while (t <= tEnd) {
            pt = ray.pointAt(t);
            const double newPhi = _func(pt);
            const double newPhiAbs = std::fabs(newPhi);

            if (newPhi * prevPhi < 0.0) {
                return true;
            }

            t += std::max(newPhiAbs, _rayMarchingResolution);
            prevPhi = newPhi;
        }
    }

    return false;
}

BoundingBox2D CustomImplicitSurface2::boundingBoxLocal() const { return _domain; }

double CustomImplicitSurface2::signedDistanceLocal(const Point2D &otherPoint) const {
    if (_func) {
        return _func(otherPoint);
    } else {
        return kMaxD;
    }
}

Vector2D CustomImplicitSurface2::closestNormalLocal(const Point2D &otherPoint) const {
    Point2D pt = closestPointLocal(otherPoint);
    Vector2D g = gradientLocal(pt);
    if (g.lengthSquared() > 0.0) {
        return g.normalized();
    } else {
        return g;
    }
}

SurfaceRayIntersection2 CustomImplicitSurface2::closestIntersectionLocal(const Ray2D &ray) const {
    SurfaceRayIntersection2 result;

    BoundingBoxRayIntersection2D intersection = _domain.closestIntersection(ray);

    if (intersection.is_intersecting) {
        double tStart, tEnd;
        if (intersection.t_far == kMaxD) {
            tStart = 0.0;
            tEnd = intersection.t_near;
        } else {
            tStart = intersection.t_near;
            tEnd = intersection.t_far;
        }

        double t = tStart;
        double tPrev = t;
        Point2D pt = ray.pointAt(t);
        double prevPhi = _func(pt);

        while (t <= tEnd) {
            pt = ray.pointAt(t);
            const double newPhi = _func(pt);
            const double newPhiAbs = std::fabs(newPhi);

            if (newPhi * prevPhi < 0.0) {
                const double frac = prevPhi / (prevPhi - newPhi);
                const double tSub = tPrev + _rayMarchingResolution * frac;

                result.isIntersecting = true;
                result.distance = tSub;
                result.point = ray.pointAt(tSub);
                result.normal = gradientLocal(result.point);
                if (result.normal.length() > 0.0) {
                    result.normal.normalize();
                }

                return result;
            }

            tPrev = t;
            t += std::max(newPhiAbs, _rayMarchingResolution);
            prevPhi = newPhi;
        }
    }

    return result;
}

Vector2D CustomImplicitSurface2::gradientLocal(const Point2D &x) const {
    double left = _func(x - Vector2D(0.5 * _resolution, 0.0));
    double right = _func(x + Vector2D(0.5 * _resolution, 0.0));
    double bottom = _func(x - Vector2D(0.0, 0.5 * _resolution));
    double top = _func(x + Vector2D(0.0, 0.5 * _resolution));

    return {(right - left) / _resolution, (top - bottom) / _resolution};
}

CustomImplicitSurface2::Builder CustomImplicitSurface2::builder() { return {}; }

CustomImplicitSurface2::Builder &CustomImplicitSurface2::Builder::withSignedDistanceFunction(
        const std::function<double(const Point2D &)> &func) {
    _func = func;
    return *this;
}

CustomImplicitSurface2::Builder &CustomImplicitSurface2::Builder::withDomain(const BoundingBox2D &domain) {
    _domain = domain;
    return *this;
}

CustomImplicitSurface2::Builder &CustomImplicitSurface2::Builder::withResolution(double resolution) {
    _resolution = resolution;
    return *this;
}

CustomImplicitSurface2::Builder &CustomImplicitSurface2::Builder::withRayMarchingResolution(double resolution) {
    _rayMarchingResolution = resolution;
    return *this;
}

CustomImplicitSurface2::Builder &CustomImplicitSurface2::Builder::withMaxNumberOfIterations(unsigned int numIter) {
    _maxNumOfIterations = numIter;
    return *this;
}

CustomImplicitSurface2 CustomImplicitSurface2::Builder::build() const {
    return CustomImplicitSurface2(_func, _domain, _resolution, _rayMarchingResolution, _maxNumOfIterations, _transform,
                                  _isNormalFlipped);
}

CustomImplicitSurface2Ptr CustomImplicitSurface2::Builder::makeShared() const {
    return {new CustomImplicitSurface2(_func, _domain, _resolution, _rayMarchingResolution, _maxNumOfIterations,
                                       _transform, _isNormalFlipped),
            [](CustomImplicitSurface2 *obj) { delete obj; }};
}
