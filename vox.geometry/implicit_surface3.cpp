// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/implicit_surface3.h"

#include "vox.geometry/level_set_utils.h"

using namespace vox;

ImplicitSurface3::ImplicitSurface3(const Transform3D &transform_, bool isNormalFlipped_)
    : Surface3(transform_, isNormalFlipped_) {}

ImplicitSurface3::ImplicitSurface3(const ImplicitSurface3 &other) = default;

ImplicitSurface3::~ImplicitSurface3() = default;

double ImplicitSurface3::signedDistance(const Point3D &otherPoint) const {
    double sd = signedDistanceLocal(transform.toLocal(otherPoint));
    return (isNormalFlipped) ? -sd : sd;
}

double ImplicitSurface3::closestDistanceLocal(const Point3D &otherPoint) const {
    return std::fabs(signedDistanceLocal(otherPoint));
}

bool ImplicitSurface3::isInsideLocal(const Point3D &otherPoint) const {
    return isInsideSdf(signedDistanceLocal(otherPoint));
}
