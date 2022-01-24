//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef plane3_inl_h
#define plane3_inl_h

namespace vox {
template<typename T>
BoundingPlane<T, 3>::BoundingPlane() : BoundingPlane(Vector3<T>(), 0) {
    
}

template<typename T>
BoundingPlane<T, 3>::BoundingPlane(const Vector3 <T> &newNormal, const T &newDistance):
normal(newNormal),
distance(newDistance) {
}

template<typename T>
BoundingPlane<T, 3>::BoundingPlane(const Point3 <T> &point0, const Point3 <T> &point1, const Point3 <T> &point2) {
    T x0 = point0.x;
    T y0 = point0.y;
    T z0 = point0.z;
    T x1 = point1.x - x0;
    T y1 = point1.y - y0;
    T z1 = point1.z - z0;
    T x2 = point2.x - x0;
    T y2 = point2.y - y0;
    T z2 = point2.z - z0;
    T yz = y1 * z2 - z1 * y2;
    T xz = z1 * x2 - x1 * z2;
    T xy = x1 * y2 - y1 * x2;
    T invPyth = 1.0 / std::sqrt(yz * yz + xz * xz + xy * xy);
    
    T x = yz * invPyth;
    T y = xz * invPyth;
    T z = xy * invPyth;
    
    normal.x = x;
    normal.y = y;
    normal.z = z;
    distance = -(x * x0 + y * y0 + z * z0);
}

template<typename T>
BoundingPlane<T, 3>::BoundingPlane(const BoundingPlane &other):
normal(other.normal),
distance(other.distance) {
}

template<typename T>
BoundingPlane<T, 3> BoundingPlane<T, 3>::normalized() const {
    T factor = 1.0 / normal.length();
    
    Vector3<T> outNormal(normal.x * factor,
                         normal.y * factor,
                         normal.z * factor);
    return BoundingPlane<T, 3>(outNormal, distance * factor);
}

template<typename T>
void BoundingPlane<T, 3>::normalize() {
    T factor = 1.0 / normal.length();
    
    normal.x *= factor;
    normal.y *= factor;
    normal.z *= factor;
    distance *= factor;
}

}

#endif /* plane3_inl_h */
