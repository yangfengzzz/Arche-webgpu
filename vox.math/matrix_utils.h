//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef matrix_utils_h
#define matrix_utils_h

#include "matrix4x4.h"
#include "quaternion.h"
#include "math_utils.h"

namespace vox {
//! Makes scale matrix.
template<typename T>
inline Matrix<T, 4, 4> makeScaleMatrix(T sx, T sy, T sz) {
    return Matrix<T, 4, 4>(sx, 0, 0, 0,
                           0, sy, 0, 0,
                           0, 0, sz, 0,
                           0, 0, 0, 1);
}

//! Makes scale matrix.
template<typename T>
inline Matrix<T, 4, 4> makeScaleMatrix(const Vector3 <T> &s) {
    return ::vox::makeScaleMatrix(s.x, s.y, s.z);
}

//! Makes rotation matrix.
//! \warning Input angle should be radian.
template<typename T>
inline Matrix<T, 4, 4> makeRotationMatrix(const Vector3 <T> &axis, T rad) {
    return Matrix<T, 4, 4>(Matrix<T, 3, 3>::makeRotationMatrix(axis, rad));
}

//! Makes translation matrix.
template<typename T>
inline Matrix<T, 4, 4> makeTranslationMatrix(const Point3 <T> &t) {
    return Matrix<T, 4, 4>(1, 0, 0, 0,
                           0, 1, 0, 0,
                           0, 0, 1, 0,
                           t.x, t.y, t.z, 1);
}

//! Makes rotation && translation matrix.
template<typename T>
inline Matrix<T, 4, 4> makeRotationTranslationMatrix(const Quaternion <T> &q, const Point3 <T> &t) {
    auto mat = q.matrix4();
    mat[12] = t.x;
    mat[13] = t.y;
    mat[14] = t.z;
    return mat;
}

//! Makes affine matrix.
template<typename T>
inline Matrix<T, 4, 4> makeAffineMatrix(const Vector3 <T> &s,
                                        const Quaternion <T> &q,
                                        const Point3 <T> &t) {
    T x = q.x;
    T y = q.y;
    T z = q.z;
    T w = q.w;
    
    T x2 = x + x;
    T y2 = y + y;
    T z2 = z + z;
    
    T xx = x * x2;
    T xy = x * y2;
    T xz = x * z2;
    T yy = y * y2;
    T yz = y * z2;
    T zz = z * z2;
    T wx = w * x2;
    T wy = w * y2;
    T wz = w * z2;
    T sx = s.x;
    T sy = s.y;
    T sz = s.z;
    
    return Matrix<T, 4, 4>((1 - (yy + zz)) * sx,
                           (xy + wz) * sx,
                           (xz - wy) * sx,
                           0,
                           
                           (xy - wz) * sy,
                           (1 - (xx + zz)) * sy,
                           (yz + wx) * sy,
                           0,
                           
                           (xz + wy) * sz,
                           (yz - wx) * sz,
                           (1 - (xx + yy)) * sz,
                           0,
                           
                           t.x,
                           t.y,
                           t.z,
                           1);
}


/**
 * Calculate a right-handed look-at matrix.
 * @param eye - The position of the viewer's eye
 * @param target - The camera look-at target
 * @param up - The camera's up vector
 */
template<typename T>
inline Matrix<T, 4, 4> makeLookAtMatrix(const Point3 <T> &eye,
                                        const Point3 <T> &target, const Vector3 <T> &up) {
    Vector3<T> zAxis = eye - target;
    zAxis.normalize();
    Vector3<T> xAxis = up.cross(zAxis);
    xAxis.normalize();
    Vector3<T> yAxis = zAxis.cross(xAxis);
    
    return Matrix<T, 4, 4>(xAxis.x,
                           yAxis.x,
                           zAxis.x,
                           0,
                           
                           xAxis.y,
                           yAxis.y,
                           zAxis.y,
                           0,
                           
                           xAxis.z,
                           yAxis.z,
                           zAxis.z,
                           0,
                           
                           -eye.dot(xAxis),
                           -eye.dot(yAxis),
                           -eye.dot(zAxis),
                           1);
}

/**
 * Calculate an orthographic projection matrix.
 * @param left - The left edge of the viewing
 * @param right - The right edge of the viewing
 * @param bottom - The bottom edge of the viewing
 * @param top - The top edge of the viewing
 * @param near - The depth of the near plane
 * @param far - The depth of the far plane
 */
template<typename T>
inline Matrix<T, 4, 4> makeOrtho(T left, T right, T bottom, T top, T near, T far) {
    T lr = (T) 1 / (left - right);
    T bt = (T) 1 / (bottom - top);
    T nf = (T) 1 / (near - far);
    
    return Matrix<T, 4, 4>(-2 * lr,
                           0,
                           0,
                           0,
                           
                           0,
                           -2 * bt,
                           0,
                           0,
                           
                           0,
                           0,
                           nf,
                           0,
                           
                           (left + right) * lr,
                           (top + bottom) * bt,
                           near * nf,
                           1);
}

/**
 * Calculate a perspective projection matrix.
 * @param fovy - Field of view in the y direction, in radians
 * @param aspect - Aspect ratio, defined as view space width divided by height
 * @param near - The depth of the near plane
 * @param far - The depth of the far plane
 */
template<typename T>
inline Matrix<T, 4, 4> makepPerspective(T fovy, T aspect, T near, T far) {
    T f = (T) 1 / std::tan(fovy / 2);
    T nf = (T) 1 / (near - far);
    
    return Matrix<T, 4, 4>(f / aspect,
                           0,
                           0,
                           0,
                           
                           0,
                           f,
                           0,
                           0,
                           
                           0,
                           0,
                           far * nf,
                           -1,
                           
                           0,
                           0,
                           far * near * nf,
                           0);
}

//MARK: - Decompose
/**
 * Decompose this matrix to translation, rotation and scale elements.
 * @param translation - Translation vector as an output parameter
 * @param rotation - Rotation quaternion as an output parameter
 * @param scale - Scale vector as an output parameter
 * @returns True if this matrix can be decomposed, false otherwise
 */
template<typename T>
bool decompose(const Matrix<T, 4, 4> &matrix,
               Point3 <T> &translation,
               Quaternion <T> &rotation,
               Vector3 <T> &scale) {
    Matrix<T, 3, 3> rm;
    const auto &m11 = matrix[0];
    const auto &m12 = matrix[1];
    const auto &m13 = matrix[2];
    const auto &m14 = matrix[3];
    const auto &m21 = matrix[4];
    const auto &m22 = matrix[5];
    const auto &m23 = matrix[6];
    const auto &m24 = matrix[7];
    const auto &m31 = matrix[8];
    const auto &m32 = matrix[9];
    const auto &m33 = matrix[10];
    const auto &m34 = matrix[11];
    
    translation.x = matrix[12];
    translation.y = matrix[13];
    translation.z = matrix[14];
    
    const auto xs = sign(m11 * m12 * m13 * m14);
    const auto ys = sign(m21 * m22 * m23 * m24);
    const auto zs = sign(m31 * m32 * m33 * m34);
    
    const auto sx = xs * std::sqrt(m11 * m11 + m12 * m12 + m13 * m13);
    const auto sy = ys * std::sqrt(m21 * m21 + m22 * m22 + m23 * m23);
    const auto sz = zs * std::sqrt(m31 * m31 + m32 * m32 + m33 * m33);
    
    scale.x = sx;
    scale.y = sy;
    scale.z = sz;
    
    if (std::abs(sx) < std::numeric_limits<T>::epsilon() ||
        std::abs(sy) < std::numeric_limits<T>::epsilon() ||
        std::abs(sz) < std::numeric_limits<T>::epsilon()) {
        rotation = Quaternion<T>::makeIdentity();
        return false;
    } else {
        const auto invSX = 1 / sx;
        const auto invSY = 1 / sy;
        const auto invSZ = 1 / sz;
        
        rm[0] = m11 * invSX;
        rm[1] = m12 * invSX;
        rm[2] = m13 * invSX;
        rm[3] = m21 * invSY;
        rm[4] = m22 * invSY;
        rm[5] = m23 * invSY;
        rm[6] = m31 * invSZ;
        rm[7] = m32 * invSZ;
        rm[8] = m33 * invSZ;
        rotation = Quaternion<T>(rm);
        return true;
    }
}

/**
 * Get rotation from this matrix.
 * @returns Rotation quaternion as an output parameter
 */
template<typename T>
Quaternion <T> getRotation(const Matrix<T, 4, 4> &matrix) {
    T trace = matrix[0] + matrix[5] + matrix[10];
    Quaternion<T> quat;
    
    if (trace > std::numeric_limits<T>::epsilon()) {
        T S = std::sqrt(trace + 1.0) * 2;
        quat.w = 0.25 * S;
        quat.x = (matrix[6] - matrix[9]) / S;
        quat.y = (matrix[8] - matrix[2]) / S;
        quat.z = (matrix[1] - matrix[4]) / S;
    } else if (matrix[0] > matrix[5] && matrix[0] > matrix[10]) {
        T S = std::sqrt(1.0 + matrix[0] - matrix[5] - matrix[10]) * 2;
        quat.w = (matrix[6] - matrix[9]) / S;
        quat.x = 0.25 * S;
        quat.y = (matrix[1] + matrix[4]) / S;
        quat.z = (matrix[8] + matrix[2]) / S;
    } else if (matrix[5] > matrix[10]) {
        T S = std::sqrt(1.0 + matrix[5] - matrix[0] - matrix[10]) * 2;
        quat.w = (matrix[8] - matrix[2]) / S;
        quat.x = (matrix[1] + matrix[4]) / S;
        quat.y = 0.25 * S;
        quat.z = (matrix[6] + matrix[9]) / S;
    } else {
        T S = std::sqrt(1.0 + matrix[10] - matrix[0] - matrix[5]) * 2;
        quat.w = (matrix[1] - matrix[4]) / S;
        quat.x = (matrix[8] + matrix[2]) / S;
        quat.y = (matrix[6] + matrix[9]) / S;
        quat.z = 0.25 * S;
    }
    
    return quat;
}

/**
 * Get scale from this matrix.
 * @returns Scale vector as an output parameter
 */
template<typename T>
Vector3 <T> getScaling(const Matrix<T, 4, 4> &matrix) {
    T m11 = matrix[0],
    m12 = matrix[1],
    m13 = matrix[2];
    T m21 = matrix[4],
    m22 = matrix[5],
    m23 = matrix[6];
    T m31 = matrix[8],
    m32 = matrix[9],
    m33 = matrix[10];
    
    Vector3<T> scaling;
    scaling.x = std::sqrt(m11 * m11 + m12 * m12 + m13 * m13);
    scaling.y = std::sqrt(m21 * m21 + m22 * m22 + m23 * m23);
    scaling.z = std::sqrt(m31 * m31 + m32 * m32 + m33 * m33);
    
    return scaling;
}

/**
 * Get translation from this matrix.
 * @returns Translation vector as an output parameter
 */
template<typename T>
Point3 <T> getTranslation(const Matrix<T, 4, 4> &matrix) {
    return Point3<T>(matrix[12], matrix[13], matrix[14]);
}

}

#endif /* matrix_utils_h */
