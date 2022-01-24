//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef INCLUDE_VOX_DETAIL_TRANSFORM2_INL_H_
#define INCLUDE_VOX_DETAIL_TRANSFORM2_INL_H_

#include "transform2.h"

#include <algorithm>
#include <cmath>

namespace vox {
template<typename T>
inline Transform2<T>::Transform2() {
}

template<typename T>
inline Transform2<T>::Transform2(const Vector2 <T> &translation,
                                 double orientation)
: _translation(translation), _orientation(orientation) {
    _cosAngle = std::cos(orientation);
    _sinAngle = std::sin(orientation);
}

template<typename T>
inline const Vector2 <T> &Transform2<T>::translation() const {
    return _translation;
}

template<typename T>
inline void Transform2<T>::setTranslation(const Vector2 <T> &translation) {
    _translation = translation;
}

template<typename T>
inline double Transform2<T>::orientation() const {
    return _orientation;
}

template<typename T>
inline void Transform2<T>::setOrientation(double orientation) {
    _orientation = orientation;
    _cosAngle = std::cos(orientation);
    _sinAngle = std::sin(orientation);
}

template<typename T>
inline Point2 <T> Transform2<T>::toLocal(const Point2 <T> &pointInWorld) const {
    // Convert to the local frame
    Point2<T> xmt = pointInWorld - _translation;
    return Point2<T>(_cosAngle * xmt.x + _sinAngle * xmt.y,
                     -_sinAngle * xmt.x + _cosAngle * xmt.y);
}

template<typename T>
inline Vector2 <T> Transform2<T>::toLocalDirection(const Vector2 <T> &dirInWorld) const {
    // Convert to the local frame
    return Vector2<T>(_cosAngle * dirInWorld.x + _sinAngle * dirInWorld.y,
                      -_sinAngle * dirInWorld.x + _cosAngle * dirInWorld.y);
}

template<typename T>
inline Ray2 <T> Transform2<T>::toLocal(const Ray2 <T> &rayInWorld) const {
    return Ray2<T>(toLocal(rayInWorld.origin),
                   toLocalDirection(rayInWorld.direction));
}

template<typename T>
inline BoundingBox2 <T> Transform2<T>::toLocal(const BoundingBox2 <T> &bboxInWorld) const {
    BoundingBox2<T> bboxInLocal;
    for (int i = 0; i < 4; ++i) {
        auto cornerInLocal = toLocal(bboxInWorld.corner(i));
        bboxInLocal.lowerCorner
        = min(bboxInLocal.lowerCorner, cornerInLocal);
        bboxInLocal.upperCorner
        = max(bboxInLocal.upperCorner, cornerInLocal);
    }
    return bboxInLocal;
}

template<typename T>
inline Point2 <T> Transform2<T>::toWorld(const Point2 <T> &pointInLocal) const {
    // Convert to the world frame
    return Point2<T>(_cosAngle * pointInLocal.x - _sinAngle * pointInLocal.y
                     + _translation.x,
                     _sinAngle * pointInLocal.x + _cosAngle * pointInLocal.y
                     + _translation.y);
}

template<typename T>
inline Vector2 <T> Transform2<T>::toWorldDirection(const Vector2 <T> &dirInLocal) const {
    // Convert to the world frame
    return Vector2<T>(_cosAngle * dirInLocal.x - _sinAngle * dirInLocal.y,
                      _sinAngle * dirInLocal.x + _cosAngle * dirInLocal.y);
}

template<typename T>
inline Ray2 <T> Transform2<T>::toWorld(const Ray2 <T> &rayInLocal) const {
    return Ray2<T>(
                   toWorld(rayInLocal.origin),
                   toWorldDirection(rayInLocal.direction));
}

template<typename T>
inline BoundingBox2 <T> Transform2<T>::toWorld(const BoundingBox2 <T> &bboxInLocal) const {
    BoundingBox2<T> bboxInWorld;
    for (int i = 0; i < 4; ++i) {
        auto cornerInWorld = toWorld(bboxInLocal.corner(i));
        bboxInWorld.lowerCorner
        = min(bboxInWorld.lowerCorner, cornerInWorld);
        bboxInWorld.upperCorner
        = max(bboxInWorld.upperCorner, cornerInWorld);
    }
    return bboxInWorld;
}

}  // namespace vox

#endif  // INCLUDE_VOX_DETAIL_TRANSFORM2_INL_H_
