//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef INCLUDE_VOX_DETAIL_QUATERNION_INL_H_
#define INCLUDE_VOX_DETAIL_QUATERNION_INL_H_

#include <limits>

namespace vox {

// Constructors
template<typename T>
inline Quaternion<T>::Quaternion() {
    setIdentity();
}

template<typename T>
inline Quaternion<T>::Quaternion(T newX, T newY, T newZ, T newW) {
    set(newX, newY, newZ, newW);
}

template<typename T>
inline Quaternion<T>::Quaternion(const std::initializer_list<T> &lst) {
    set(lst);
}

template<typename T>
inline Quaternion<T>::Quaternion(const Vector3 <T> &axis, T angle) {
    set(axis, angle);
}

template<typename T>
inline Quaternion<T>::Quaternion(const Vector3 <T> &from, const Vector3 <T> &to) {
    set(from, to);
}

template<typename T>
inline Quaternion<T>::Quaternion(const Vector3 <T> &rotationBasis0,
                                 const Vector3 <T> &rotationBasis1,
                                 const Vector3 <T> &rotationBasis2) {
    set(rotationBasis0, rotationBasis1, rotationBasis2);
}

template<typename T>
inline Quaternion<T>::Quaternion(const Matrix3x3 <T> &matrix) {
    set(matrix);
}

template<typename T>
inline Quaternion<T>::Quaternion(const Quaternion &other) {
    set(other);
}


// Basic setters
template<typename T>
inline void Quaternion<T>::set(const Quaternion &other) {
    set(other.x, other.y, other.z, other.w);
}

template<typename T>
inline void Quaternion<T>::set(T newX, T newY, T newZ, T newW) {
    x = newX;
    y = newY;
    z = newZ;
    w = newW;
}

template<typename T>
inline void Quaternion<T>::set(const std::initializer_list<T> &lst) {
    assert(lst.size() == 4);
    
    auto inputElem = lst.begin();
    x = *inputElem;
    y = *(++inputElem);
    z = *(++inputElem);
    w = *(++inputElem);
}

template<typename T>
inline void Quaternion<T>::set(const Vector3 <T> &axis, T angle) {
    static const T eps = std::numeric_limits<T>::epsilon();
    
    T axisLengthSquared = axis.lengthSquared();
    
    if (axisLengthSquared < eps) {
        setIdentity();
    } else {
        Vector3<T> normalizedAxis = axis.normalized();
        T s = std::sin(angle / 2);
        
        x = normalizedAxis.x * s;
        y = normalizedAxis.y * s;
        z = normalizedAxis.z * s;
        w = std::cos(angle / 2);
    }
}

template<typename T>
inline void Quaternion<T>::set(const Vector3 <T> &from, const Vector3 <T> &to) {
    static const T eps = std::numeric_limits<T>::epsilon();
    
    Vector3<T> axis = from.cross(to);
    
    T fromLengthSquared = from.lengthSquared();
    T toLengthSquared = to.lengthSquared();
    
    if (fromLengthSquared < eps ||
        toLengthSquared < eps) {
        setIdentity();
    } else {
        T axisLengthSquared = axis.lengthSquared();
        
        // In case two vectors are exactly the opposite, pick orthogonal vector
        // for axis.
        if (axisLengthSquared < eps) {
            axis = std::get<0>(from.tangential());
        }
        
        set(axis.x, axis.y, axis.z, from.dot(to));
        w += l2Norm();
        
        normalize();
    }
}

template<typename T>
inline void Quaternion<T>::set(const Vector3 <T> &rotationBasis0,
                               const Vector3 <T> &rotationBasis1,
                               const Vector3 <T> &rotationBasis2) {
    Matrix3x3<T> matrix3;
    
    matrix3.setColumn(0, rotationBasis0.normalized());
    matrix3.setColumn(1, rotationBasis1.normalized());
    matrix3.setColumn(2, rotationBasis2.normalized());
    
    set(matrix3);
}

template<typename T>
inline void Quaternion<T>::set(const Matrix3x3 <T> &m) {
    static const T eps = std::numeric_limits<T>::epsilon();
    static const T quater = static_cast<T>(0.25);
    
    T onePlusTrace = m.trace() + 1;
    
    if (onePlusTrace > eps) {
        T S = std::sqrt(onePlusTrace) * 2;
        w = quater * S;
        x = (m(2, 1) - m(1, 2)) / S;
        y = (m(0, 2) - m(2, 0)) / S;
        z = (m(1, 0) - m(0, 1)) / S;
    } else if (m(0, 0) > m(1, 1) && m(0, 0) > m(2, 2)) {
        T S = std::sqrt(1 + m(0, 0) - m(1, 1) - m(2, 2)) * 2;
        w = (m(2, 1) - m(1, 2)) / S;
        x = quater * S;
        y = (m(0, 1) + m(1, 0)) / S;
        z = (m(0, 2) + m(2, 0)) / S;
    } else if (m(1, 1) > m(2, 2)) {
        T S = std::sqrt(1 + m(1, 1) - m(0, 0) - m(2, 2)) * 2;
        w = (m(0, 2) - m(2, 0)) / S;
        x = (m(0, 1) + m(1, 0)) / S;
        y = quater * S;
        z = (m(1, 2) + m(2, 1)) / S;
    } else {
        T S = std::sqrt(1 + m(2, 2) - m(0, 0) - m(1, 1)) * 2;
        w = (m(1, 0) - m(0, 1)) / S;
        x = (m(0, 2) + m(2, 0)) / S;
        y = (m(1, 2) + m(2, 1)) / S;
        z = quater * S;
    }
}


// Basic getters
template<typename T>
template<typename U>
Quaternion <U> Quaternion<T>::castTo() const {
    return Quaternion<U>(static_cast<U>(x),
                         static_cast<U>(y),
                         static_cast<U>(z),
                         static_cast<U>(w));
}

//! Returns normalized quaternion.
template<typename T>
Quaternion <T> Quaternion<T>::normalized() const {
    Quaternion q(*this);
    q.normalize();
    return q;
}

template<typename T>
Quaternion<T> Quaternion<T>::add(const Quaternion &v) const {
    return Quaternion(x + v.x, y + v.y, z + v.z, w + v.w);
}

template<typename T>
Quaternion<T> Quaternion<T>::mul(T v) const {
    return Quaternion(x * v, y * v, z * v, w * v);
}

// Binary operator methods - new instance = this instance (+) input
template<typename T>
inline Point3 <T> Quaternion<T>::mul(const Point3 <T> &v) const {
    // calculate quat * vec
    T ix = w * v.x + y * v.z - z * v.y;
    T iy = w * v.y + z * v.x - x * v.z;
    T iz = w * v.z + x * v.y - y * v.x;
    T iw = -x * v.x - y * v.y - z * v.z;
    
    // calculate result * inverse quat
    return Point3<T>(ix * w - iw * x - iy * z + iz * y,
                     iy * w - iw * y - iz * x + ix * z,
                     iz * w - iw * z - ix * y + iy * x);
}

// Binary operator methods - new instance = this instance (+) input
template<typename T>
inline Vector3 <T> Quaternion<T>::mul(const Vector3 <T> &v) const {
    // calculate quat * vec
    T ix = w * v.x + y * v.z - z * v.y;
    T iy = w * v.y + z * v.x - x * v.z;
    T iz = w * v.z + x * v.y - y * v.x;
    T iw = -x * v.x - y * v.y - z * v.z;
    
    // calculate result * inverse quat
    return Vector3<T>(ix * w - iw * x - iy * z + iz * y,
                      iy * w - iw * y - iz * x + ix * z,
                      iz * w - iw * z - ix * y + iy * x);
}

// Binary operator methods - new instance = this instance (+) input
template<typename T>
inline Vector4 <T> Quaternion<T>::mul(const Vector4 <T> &v) const {
    // calculate quat * vec
    T ix = w * v.x + y * v.z - z * v.y;
    T iy = w * v.y + z * v.x - x * v.z;
    T iz = w * v.z + x * v.y - y * v.x;
    T iw = -x * v.x - y * v.y - z * v.z;
    
    // calculate result * inverse quat
    return Vector4<T>(ix * w - iw * x - iy * z + iz * y,
                      iy * w - iw * y - iz * x + ix * z,
                      iz * w - iw * z - ix * y + iy * x,
                      v.w);
}

template<typename T>
inline Quaternion <T> Quaternion<T>::mul(const Quaternion &other) const {
    return Quaternion(w * other.x + x * other.w + y * other.z - z * other.y,
                      w * other.y - x * other.z + y * other.w + z * other.x,
                      w * other.z + x * other.y - y * other.x + z * other.w,
                      w * other.w - x * other.x - y * other.y - z * other.z);
}

template<typename T>
inline T Quaternion<T>::dot(const Quaternion <T> &other) const {
    return w * other.w + x * other.x + y * other.y + z * other.z;
}

// Binary operator methods - new instance = input (+) this instance
template<typename T>
inline Quaternion <T> Quaternion<T>::rmul(const Quaternion &other) const {
    return Quaternion(other.w * x + other.x * w + other.y * z - other.z * y,
                      other.w * y - other.x * z + other.y * w + other.z * x,
                      other.w * z + other.x * y - other.y * x + other.z * w,
                      other.w * w - other.x * x - other.y * y - other.z * z);
}


template<typename T>
void Quaternion<T>::iadd(const Quaternion &v) {
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
}

template<typename T>
void Quaternion<T>::imul(T v) {
    x *= v;
    y *= v;
    z *= v;
    w *= v;
}

// Augmented operator methods - this instance (+)= input
template<typename T>
inline void Quaternion<T>::imul(const Quaternion &other) {
    *this = mul(other);
}


// Modifiers
template<typename T>
inline void Quaternion<T>::setIdentity() {
    set(0, 0, 0, 1);
}

template<typename T>
inline void Quaternion<T>::rotate(T angleInRadians) {
    Vector3<T> axis;
    T currentAngle;
    
    getAxisAngle(&axis, &currentAngle);
    
    currentAngle += angleInRadians;
    
    set(axis, currentAngle);
}

template<typename T>
inline void Quaternion<T>::normalize() {
    T norm = l2Norm();
    
    if (norm > 0) {
        w /= norm;
        x /= norm;
        y /= norm;
        z /= norm;
    }
}

template<typename T>
inline Quaternion <T> Quaternion<T>::rotateX(T rad) const {
    rad *= 0.5;
    T bx = std::sin(rad);
    T bw = std::cos(rad);
    
    return Quaternion<T>(x * bw + w * bx,
                         y * bw + z * bx,
                         z * bw - y * bx,
                         w * bw - x * bx);
}

template<typename T>
inline void Quaternion<T>::rotateX(T rad) {
    const T x = this->x;
    const T y = this->y;
    const T z = this->z;
    const T w = this->w;
    
    rad *= 0.5;
    T bx = std::sin(rad);
    T bw = std::cos(rad);
    
    this->x = x * bw + w * bx;
    this->y = y * bw + z * bx;
    this->z = z * bw - y * bx;
    this->w = w * bw - x * bx;
}

template<typename T>
inline Quaternion <T> Quaternion<T>::rotateY(T rad) const {
    rad *= 0.5;
    T by = std::sin(rad);
    T bw = std::cos(rad);
    
    return Quaternion<T>(x * bw - z * by,
                         y * bw + w * by,
                         z * bw + x * by,
                         w * bw - y * by);
}

template<typename T>
inline void Quaternion<T>::rotateY(T rad) {
    const T x = this->x;
    const T y = this->y;
    const T z = this->z;
    const T w = this->w;
    
    rad *= 0.5;
    T by = std::sin(rad);
    T bw = std::cos(rad);
    
    this->x = x * bw - z * by;
    this->y = y * bw + w * by;
    this->z = z * bw + x * by;
    this->w = w * bw - y * by;
}

template<typename T>
inline Quaternion <T> Quaternion<T>::rotateZ(T rad) const {
    rad *= 0.5;
    T bz = std::sin(rad);
    T bw = std::cos(rad);
    
    return Quaternion<T>(x * bw + y * bz,
                         y * bw - x * bz,
                         z * bw + w * bz,
                         w * bw - z * bz);
}

template<typename T>
inline void Quaternion<T>::rotateZ(T rad) {
    const T x = this->x;
    const T y = this->y;
    const T z = this->z;
    const T w = this->w;
    
    rad *= 0.5;
    T bz = std::sin(rad);
    T bw = std::cos(rad);
    
    this->x = x * bw + y * bz;
    this->y = y * bw - x * bz;
    this->z = z * bw + w * bz;
    this->w = w * bw - z * bz;
}

template<typename T>
void Quaternion<T>::rotateAxisAngle(const Vector3<T>& axis, T rad) {
    Quaternion _tempQuat1 = Quaternion(axis, rad);
    *this *= _tempQuat1;
}

// Complex getters
template<typename T>
inline Vector3 <T> Quaternion<T>::axis() const {
    Vector3<T> result(x, y, z);
    result.normalize();
    
    if (2 * std::acos(w) < pi<T>()) {
        return result;
    } else {
        return -result;
    }
}

template<typename T>
inline T Quaternion<T>::angle() const {
    T result = 2 * std::acos(w);
    
    if (result < pi<T>()) {
        return result;
    } else {
        // Wrap around
        return 2 * pi<T>() - result;
    }
}

template<typename T>
inline void Quaternion<T>::getAxisAngle(Vector3 <T> *axis, T *angle) const {
    axis->set(x, y, z);
    axis->normalize();
    *angle = 2 * std::acos(w);
    
    if (*angle > pi<T>()) {
        // Wrap around
        (*axis) = -(*axis);
        *angle = 2 * pi<T>() - (*angle);
    }
}

template<typename T>
inline Vector3 <T> Quaternion<T>::toEuler() const {
    Vector3<T> euler = toYawPitchRoll();
    T t = euler.x;
    euler.x = euler.y;
    euler.y = t;
    return euler;
}

template<typename T>
inline Vector3 <T> Quaternion<T>::toYawPitchRoll() const {
    T xx = x * x;
    T yy = y * y;
    T zz = z * z;
    T xy = x * y;
    T zw = z * w;
    T zx = z * x;
    T yw = y * w;
    T yz = y * z;
    T xw = x * w;
    
    Vector3<T> euler;
    euler.y = std::asin(2.0 * (xw - yz));
    if (std::cos(euler.y) > std::numeric_limits<T>::epsilon()) {
        euler.z = std::atan2(2.0 * (xy + zw), 1.0 - 2.0 * (zz + xx));
        euler.x = std::atan2(2.0 * (zx + yw), 1.0 - 2.0 * (yy + xx));
    } else {
        euler.z = std::atan2(-2.0 * (xy - zw), 1.0 - 2.0 * (yy + zz));
        euler.x = 0.0;
    }
    
    return euler;
}

template<typename T>
inline Quaternion <T> Quaternion<T>::conjugate() const {
    return Quaternion<T>(-x, -y, -z, w);
}

template<typename T>
inline T Quaternion<T>::length() const {
    return std::sqrt(x * x + y * y + z * z + w * w);
}

template<typename T>
inline T Quaternion<T>::lengthSquared() const {
    return x * x + y * y + z * z + w * w;
}

template<typename T>
inline Quaternion <T> Quaternion<T>::inverse() const {
    T denom = w * w + x * x + y * y + z * z;
    return Quaternion(-x / denom, -y / denom, -z / denom, w / denom);
}

template<typename T>
inline Matrix3x3 <T> Quaternion<T>::matrix3() const {
    T _2xx = 2 * x * x;
    T _2yy = 2 * y * y;
    T _2zz = 2 * z * z;
    T _2xy = 2 * x * y;
    T _2xz = 2 * x * z;
    T _2xw = 2 * x * w;
    T _2yz = 2 * y * z;
    T _2yw = 2 * y * w;
    T _2zw = 2 * z * w;
    
    Matrix3x3<T> m(1 - _2yy - _2zz, _2xy + _2zw, _2xz - _2yw,
                   _2xy - _2zw, 1 - _2zz - _2xx, _2yz + _2xw,
                   _2xz + _2yw, _2yz - _2xw, 1 - _2yy - _2xx);
    
    return m;
}

template<typename T>
inline Matrix4x4 <T> Quaternion<T>::matrix4() const {
    T _2xx = 2 * x * x;
    T _2yy = 2 * y * y;
    T _2zz = 2 * z * z;
    T _2xy = 2 * x * y;
    T _2xz = 2 * x * z;
    T _2xw = 2 * x * w;
    T _2yz = 2 * y * z;
    T _2yw = 2 * y * w;
    T _2zw = 2 * z * w;
    
    Matrix4x4<T> m(1 - _2yy - _2zz, _2xy + _2zw, _2xz - _2yw, 0,
                   _2xy - _2zw, 1 - _2zz - _2xx, _2yz + _2xw, 0,
                   _2xz + _2yw, _2yz - _2xw, 1 - _2yy - _2xx, 0,
                   0, 0, 0, 1);
    
    return m;
}

template<typename T>
inline T Quaternion<T>::l2Norm() const {
    return std::sqrt(w * w + x * x + y * y + z * z);
}

// Setter operators
template<typename T>
inline Quaternion <T> &Quaternion<T>::operator=(const Quaternion &other) {
    set(other);
    return *this;
}

template<typename T>
Quaternion<T> &Quaternion<T>::operator+=(const Quaternion &v) {
    iadd(v);
    return (*this);
}

template<typename T>
Quaternion<T> &Quaternion<T>::operator*=(T v) {
    imul(v);
    return (*this);
}

template<typename T>
inline Quaternion <T> &Quaternion<T>::operator*=(const Quaternion &other) {
    imul(other);
    return *this;
}


// Getter operators
template<typename T>
inline T &Quaternion<T>::operator[](size_t i) {
    return (&w)[i];
}

template<typename T>
inline const T &Quaternion<T>::operator[](size_t i) const {
    return (&w)[i];
}

template<typename T>
bool Quaternion<T>::operator==(const Quaternion &other) const {
    return w == other.w &&
    x == other.x &&
    y == other.y &&
    z == other.z;
}

template<typename T>
bool Quaternion<T>::operator!=(const Quaternion &other) const {
    return w != other.w ||
    x != other.x ||
    y != other.y ||
    z != other.z;
}

template<typename T>
Quaternion <T> Quaternion<T>::makeIdentity() {
    return Quaternion();
}

template<typename T>
Quaternion <T> Quaternion<T>::makeRotationEuler(T pitch, T yaw, T roll) {
    return Quaternion<T>::makeRotationYawPitchRoll(yaw, pitch, roll);
}

template<typename T>
Quaternion <T> Quaternion<T>::makeRotationYawPitchRoll(T yaw, T pitch, T roll) {
    T halfRoll = roll * 0.5;
    T halfPitch = pitch * 0.5;
    T halfYaw = yaw * 0.5;
    
    T sinRoll = std::sin(halfRoll);
    T cosRoll = std::cos(halfRoll);
    T sinPitch = std::sin(halfPitch);
    T cosPitch = std::cos(halfPitch);
    T sinYaw = std::sin(halfYaw);
    T cosYaw = std::cos(halfYaw);
    
    T cosYawPitch = cosYaw * cosPitch;
    T sinYawPitch = sinYaw * sinPitch;
    
    return Quaternion<T>(cosYaw * sinPitch * cosRoll + sinYaw * cosPitch * sinRoll,
                         sinYaw * cosPitch * cosRoll - cosYaw * sinPitch * sinRoll,
                         cosYawPitch * sinRoll - sinYawPitch * cosRoll,
                         cosYawPitch * cosRoll + sinYawPitch * sinRoll);
}

template<typename T>
Quaternion <T> Quaternion<T>::makeRotationX(T rad) {
    rad *= 0.5;
    T s = std::sin(rad);
    T c = std::cos(rad);
    return Quaternion<T>(s, 0, 0, c);
}

template<typename T>
Quaternion <T> Quaternion<T>::makeRotationY(T rad) {
    rad *= 0.5;
    T s = std::sin(rad);
    T c = std::cos(rad);
    return Quaternion<T>(0, s, 0, c);
}

template<typename T>
Quaternion <T> Quaternion<T>::makeRotationZ(T rad) {
    rad *= 0.5;
    T s = std::sin(rad);
    T c = std::cos(rad);
    return Quaternion<T>(0, 0, s, c);
}

template<typename T>
inline Quaternion <T> slerp(const Quaternion <T> &a,
                            const Quaternion <T> &b,
                            T t) {
    static const double threshold = 0.01;
    static const T eps = std::numeric_limits<T>::epsilon();
    
    T cosHalfAngle = a.dot(b);
    T weightA, weightB;
    
    // For better accuracy, return lerp result when a and b are close enough.
    if (1.0 - std::fabs(cosHalfAngle) < threshold) {
        weightA = 1.0 - t;
        weightB = t;
    } else {
        T halfAngle = std::acos(cosHalfAngle);
        T sinHalfAngle = std::sqrt(1 - cosHalfAngle * cosHalfAngle);
        
        // In case of angle ~ 180, pick middle value.
        // If not, perform slerp.
        if (std::fabs(sinHalfAngle) < eps) {
            weightA = static_cast<T>(0.5);
            weightB = static_cast<T>(0.5);
        } else {
            weightA = std::sin((1 - t) * halfAngle) / sinHalfAngle;
            weightB = std::sin(t * halfAngle) / sinHalfAngle;
        }
    }
    
    return Quaternion<T>(weightA * a.x + weightB * b.x,
                         weightA * a.y + weightB * b.y,
                         weightA * a.z + weightB * b.z,
                         weightA * a.w + weightB * b.w);
}


// Operator overloadings
template<typename T>
Quaternion<T> operator+(const Quaternion<T> &a, const Quaternion<T> &b) {
    return a.add(b);
}

template<typename T>
Quaternion<T> operator*(const Quaternion<T> &a, T b) {
    return a.mul(b);
}

template<typename T>
Quaternion<T> operator*(T a, const Quaternion<T> &b) {
    return b.mul(a);
}

template<typename T>
inline Point<T, 3> operator*(const Quaternion <T> &q, const Point<T, 3> &v) {
    return q.mul(v);
}

template<typename T>
inline Vector<T, 3> operator*(const Quaternion <T> &q, const Vector<T, 3> &v) {
    return q.mul(v);
}

template<typename T>
inline Vector<T, 4> operator*(const Quaternion <T> &q, const Vector<T, 4> &v) {
    return q.mul(v);
}

template<typename T>
inline Quaternion <T> operator*(const Quaternion <T> &a, const Quaternion <T> &b) {
    return a.mul(b);
}

}  // namespace vox

#endif  // INCLUDE_VOX_DETAIL_QUATERNION_INL_H_
