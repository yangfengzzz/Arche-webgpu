//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef INCLUDE_VOX_DETAIL_RAY2_INL_H_
#define INCLUDE_VOX_DETAIL_RAY2_INL_H_

namespace vox {

template<typename T>
Ray<T, 2>::Ray() : Ray(Point2<T>(), Vector2<T>(1, 0)) {
}

template<typename T>
Ray<T, 2>::Ray(const Point2 <T> &newOrigin,
               const Vector2 <T> &newDirection) :
origin(newOrigin),
direction(newDirection.normalized()) {
}

template<typename T>
Ray<T, 2>::Ray(const Ray &other) :
origin(other.origin),
direction(other.direction) {
}

template<typename T>
Point2 <T> Ray<T, 2>::pointAt(T t) const {
    return origin + t * direction;
}

}  // namespace vox

#endif  // INCLUDE_VOX_DETAIL_RAY2_INL_H_
