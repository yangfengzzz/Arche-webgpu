//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef INCLUDE_VOX_DETAIL_TRANSFORM3_INL_H_
#define INCLUDE_VOX_DETAIL_TRANSFORM3_INL_H_

#include "transform3.h"

#include <algorithm>
#include <cmath>

namespace vox {
template<typename T>
inline Transform3<T>::Transform3() {
}

template<typename T>
inline Transform3<T>::Transform3(const Vector3 <T> &translation,
                                 const Quaternion <T> &orientation) {
    setTranslation(translation);
    setOrientation(orientation);
}

template<typename T>
inline const Vector3 <T> &Transform3<T>::translation() const {
    return _translation;
}

template<typename T>
inline void Transform3<T>::setTranslation(const Vector3 <T> &translation) {
    _translation = translation;
}

template<typename T>
inline const Quaternion <T> &Transform3<T>::orientation() const {
    return _orientation;
}

template<typename T>
inline void Transform3<T>::setOrientation(const Quaternion <T> &orientation) {
    _orientation = orientation;
    _orientationMat3 = orientation.matrix3();
    _inverseOrientationMat3 = orientation.inverse().matrix3();
}

template<typename T>
inline Point3 <T> Transform3<T>::toLocal(const Point3 <T> &pointInWorld) const {
    return _inverseOrientationMat3 * (pointInWorld - _translation);
}

template<typename T>
inline Vector3 <T> Transform3<T>::toLocalDirection(const Vector3 <T> &dirInWorld) const {
    return _inverseOrientationMat3 * dirInWorld;
}

template<typename T>
inline Ray3 <T> Transform3<T>::toLocal(const Ray3 <T> &rayInWorld) const {
    return Ray3<T>(
                   toLocal(rayInWorld.origin),
                   toLocalDirection(rayInWorld.direction));
}

template<typename T>
inline BoundingBox3 <T> Transform3<T>::toLocal(const BoundingBox3 <T> &bboxInWorld) const {
    BoundingBox3<T> bboxInLocal;
    for (int i = 0; i < 8; ++i) {
        auto cornerInLocal = toLocal(bboxInWorld.corner(i));
        bboxInLocal.lowerCorner
        = min(bboxInLocal.lowerCorner, cornerInLocal);
        bboxInLocal.upperCorner
        = max(bboxInLocal.upperCorner, cornerInLocal);
    }
    return bboxInLocal;
}

template<typename T>
inline Point3 <T> Transform3<T>::toWorld(const Point3 <T> &pointInLocal) const {
    return (_orientationMat3 * pointInLocal) + _translation;
}

template<typename T>
inline Vector3 <T> Transform3<T>::toWorldDirection(const Vector3 <T> &dirInLocal) const {
    return _orientationMat3 * dirInLocal;
}

template<typename T>
inline Ray3 <T> Transform3<T>::toWorld(const Ray3 <T> &rayInLocal) const {
    return Ray3<T>(toWorld(rayInLocal.origin),
                   toWorldDirection(rayInLocal.direction));
}

template<typename T>
inline BoundingBox3 <T> Transform3<T>::toWorld(const BoundingBox3 <T> &bboxInLocal) const {
    BoundingBox3<T> bboxInWorld;
    for (int i = 0; i < 8; ++i) {
        auto cornerInWorld = toWorld(bboxInLocal.corner(i));
        bboxInWorld.lowerCorner
        = min(bboxInWorld.lowerCorner, cornerInWorld);
        bboxInWorld.upperCorner
        = max(bboxInWorld.upperCorner, cornerInWorld);
    }
    return bboxInWorld;
}

}  // namespace vox

#endif  // INCLUDE_VOX_DETAIL_TRANSFORM3_INL_H_
