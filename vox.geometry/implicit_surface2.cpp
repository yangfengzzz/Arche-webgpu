// Copyright (c) 2022 Feng Yang
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include "vox.geometry/implicit_surface2.h"

#include "vox.geometry/level_set_utils.h"

using namespace vox;

ImplicitSurface2::ImplicitSurface2(const Transform2D &transform, bool isNormalFlipped)
    : Surface2(transform, isNormalFlipped) {}

ImplicitSurface2::ImplicitSurface2(const ImplicitSurface2 &other) = default;

ImplicitSurface2::~ImplicitSurface2() = default;

double ImplicitSurface2::signedDistance(const Point2D &otherPoint) const {
    double sd = signedDistanceLocal(transform.toLocal(otherPoint));
    return (isNormalFlipped) ? -sd : sd;
}

double ImplicitSurface2::closestDistanceLocal(const Point2D &otherPoint) const {
    return std::fabs(signedDistanceLocal(otherPoint));
}

bool ImplicitSurface2::isInsideLocal(const Point2D &otherPoint) const {
    return isInsideSdf(signedDistanceLocal(otherPoint));
}
