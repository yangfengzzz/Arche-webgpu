//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.geometry/svd.h"
#include "vox.math/math_utils.h"
#include "vox.math/matrix4x4.h"
#include "vox.math/quaternion.h"

namespace vox {
//! Makes scale matrix.
template <typename T>
inline Matrix<T, 4, 4> makeScaleMatrix(T sx, T sy, T sz) {
    return Matrix<T, 4, 4>(sx, 0, 0, 0, 0, sy, 0, 0, 0, 0, sz, 0, 0, 0, 0, 1);
}

//! Makes scale matrix.
template <typename T>
inline Matrix<T, 4, 4> makeScaleMatrix(const Vector3<T> &s) {
    return ::vox::makeScaleMatrix(s.x, s.y, s.z);
}

//! Makes rotation matrix.
//! \warning Input angle should be radian.
template <typename T>
inline Matrix<T, 4, 4> makeRotationMatrix(const Vector3<T> &axis, T rad) {
    return Matrix<T, 4, 4>(Matrix<T, 3, 3>::makeRotationMatrix(axis, rad));
}

//! Makes translation matrix.
template <typename T>
inline Matrix<T, 4, 4> makeTranslationMatrix(const Point3<T> &t) {
    return Matrix<T, 4, 4>(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, t.x, t.y, t.z, 1);
}

//! Makes rotation && translation matrix.
template <typename T>
inline Matrix<T, 4, 4> makeRotationTranslationMatrix(const Quaternion<T> &q, const Point3<T> &t) {
    auto mat = q.matrix4();
    mat[12] = t.x;
    mat[13] = t.y;
    mat[14] = t.z;
    return mat;
}

//! Makes affine matrix.
template <typename T>
inline Matrix<T, 4, 4> makeAffineMatrix(const Vector3<T> &s, const Quaternion<T> &q, const Point3<T> &t) {
    T x = q.x;
    T y = q.y;
    T z = q.z;
    T w = q.w;

    T x_2 = x + x;
    T y_2 = y + y;
    T z_2 = z + z;

    T xx = x * x_2;
    T xy = x * y_2;
    T xz = x * z_2;
    T yy = y * y_2;
    T yz = y * z_2;
    T zz = z * z_2;
    T wx = w * x_2;
    T wy = w * y_2;
    T wz = w * z_2;
    T sx = s.x;
    T sy = s.y;
    T sz = s.z;

    return Matrix<T, 4, 4>((1 - (yy + zz)) * sx, (xy + wz) * sx, (xz - wy) * sx, 0,

                           (xy - wz) * sy, (1 - (xx + zz)) * sy, (yz + wx) * sy, 0,

                           (xz + wy) * sz, (yz - wx) * sz, (1 - (xx + yy)) * sz, 0,

                           t.x, t.y, t.z, 1);
}

/**
 * Calculate a right-handed look-at matrix.
 * @param eye - The position of the viewer's eye
 * @param target - The camera look-at target
 * @param up - The camera's up vector
 */
template <typename T>
inline Matrix<T, 4, 4> makeLookAtMatrix(const Point3<T> &eye, const Point3<T> &target, const Vector3<T> &up) {
    Vector3<T> z_axis = eye - target;
    z_axis.normalize();
    Vector3<T> x_axis = up.cross(z_axis);
    x_axis.normalize();
    Vector3<T> y_axis = z_axis.cross(x_axis);

    return Matrix<T, 4, 4>(x_axis.x, y_axis.x, z_axis.x, 0,

                           x_axis.y, y_axis.y, z_axis.y, 0,

                           x_axis.z, y_axis.z, z_axis.z, 0,

                           -eye.dot(x_axis), -eye.dot(y_axis), -eye.dot(z_axis), 1);
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
template <typename T>
inline Matrix<T, 4, 4> makeOrtho(T left, T right, T bottom, T top, T near, T far) {
    T lr = (T)1 / (left - right);
    T bt = (T)1 / (bottom - top);
    T nf = (T)1 / (near - far);

    return Matrix<T, 4, 4>(-2 * lr, 0, 0, 0,

                           0, -2 * bt, 0, 0,

                           0, 0, nf, 0,

                           (left + right) * lr, (top + bottom) * bt, near * nf, 1);
}

/**
 * Calculate a perspective projection matrix.
 * @param fovy - Field of view in the y direction, in radians
 * @param aspect - Aspect ratio, defined as view space width divided by height
 * @param near - The depth of the near plane
 * @param far - The depth of the far plane
 */
template <typename T>
inline Matrix<T, 4, 4> makePerspective(T fovy, T aspect, T near, T far) {
    T f = (T)1 / std::tan(fovy / 2);
    T nf = (T)1 / (near - far);

    return Matrix<T, 4, 4>(f / aspect, 0, 0, 0,

                           0, f, 0, 0,

                           0, 0, far * nf, -1,

                           0, 0, far * near * nf, 0);
}

// MARK: - Decompose
/**
 * Decompose this matrix to translation, rotation and scale elements.
 * @param translation - Translation vector as an output parameter
 * @param rotation - Rotation quaternion as an output parameter
 * @param scale - Scale vector as an output parameter
 * @returns True if this matrix can be decomposed, false otherwise
 */
template <typename T>
bool decompose(const Matrix<T, 4, 4> &matrix, Point3<T> &translation, Quaternion<T> &rotation, Vector3<T> &scale) {
    Matrix<T, 3, 3> rm;
    const auto &m_11 = matrix[0];
    const auto &m_12 = matrix[1];
    const auto &m_13 = matrix[2];
    const auto &m_14 = matrix[3];
    const auto &m_21 = matrix[4];
    const auto &m_22 = matrix[5];
    const auto &m_23 = matrix[6];
    const auto &m_24 = matrix[7];
    const auto &m_31 = matrix[8];
    const auto &m_32 = matrix[9];
    const auto &m_33 = matrix[10];
    const auto &m_34 = matrix[11];

    translation.x = matrix[12];
    translation.y = matrix[13];
    translation.z = matrix[14];

    const auto kXs = sign(m_11 * m_12 * m_13 * m_14);
    const auto kYs = sign(m_21 * m_22 * m_23 * m_24);
    const auto kZs = sign(m_31 * m_32 * m_33 * m_34);

    const auto kSx = kXs * std::sqrt(m_11 * m_11 + m_12 * m_12 + m_13 * m_13);
    const auto kSy = kYs * std::sqrt(m_21 * m_21 + m_22 * m_22 + m_23 * m_23);
    const auto kSz = kZs * std::sqrt(m_31 * m_31 + m_32 * m_32 + m_33 * m_33);

    scale.x = kSx;
    scale.y = kSy;
    scale.z = kSz;

    if (std::abs(kSx) < std::numeric_limits<T>::epsilon() || std::abs(kSy) < std::numeric_limits<T>::epsilon() ||
        std::abs(kSz) < std::numeric_limits<T>::epsilon()) {
        rotation = Quaternion<T>::makeIdentity();
        return false;
    } else {
        const auto kInvSx = 1 / kSx;
        const auto kInvSy = 1 / kSy;
        const auto kInvSz = 1 / kSz;

        rm[0] = m_11 * kInvSx;
        rm[1] = m_12 * kInvSx;
        rm[2] = m_13 * kInvSx;
        rm[3] = m_21 * kInvSy;
        rm[4] = m_22 * kInvSy;
        rm[5] = m_23 * kInvSy;
        rm[6] = m_31 * kInvSz;
        rm[7] = m_32 * kInvSz;
        rm[8] = m_33 * kInvSz;
        rotation = Quaternion<T>(rm);
        return true;
    }
}

/**
 * Get rotation from this matrix.
 * @returns Rotation quaternion as an output parameter
 */
template <typename T>
Quaternion<T> getRotation(const Matrix<T, 4, 4> &matrix) {
    T trace = matrix[0] + matrix[5] + matrix[10];
    Quaternion<T> quat;

    if (trace > std::numeric_limits<T>::epsilon()) {
        T s = std::sqrt(trace + 1.0) * 2;
        quat.w = 0.25 * s;
        quat.x = (matrix[6] - matrix[9]) / s;
        quat.y = (matrix[8] - matrix[2]) / s;
        quat.z = (matrix[1] - matrix[4]) / s;
    } else if (matrix[0] > matrix[5] && matrix[0] > matrix[10]) {
        T s = std::sqrt(1.0 + matrix[0] - matrix[5] - matrix[10]) * 2;
        quat.w = (matrix[6] - matrix[9]) / s;
        quat.x = 0.25 * s;
        quat.y = (matrix[1] + matrix[4]) / s;
        quat.z = (matrix[8] + matrix[2]) / s;
    } else if (matrix[5] > matrix[10]) {
        T s = std::sqrt(1.0 + matrix[5] - matrix[0] - matrix[10]) * 2;
        quat.w = (matrix[8] - matrix[2]) / s;
        quat.x = (matrix[1] + matrix[4]) / s;
        quat.y = 0.25 * s;
        quat.z = (matrix[6] + matrix[9]) / s;
    } else {
        T s = std::sqrt(1.0 + matrix[10] - matrix[0] - matrix[5]) * 2;
        quat.w = (matrix[1] - matrix[4]) / s;
        quat.x = (matrix[8] + matrix[2]) / s;
        quat.y = (matrix[6] + matrix[9]) / s;
        quat.z = 0.25 * s;
    }

    return quat;
}

/**
 * Get scale from this matrix.
 * @returns Scale vector as an output parameter
 */
template <typename T>
Vector3<T> getScaling(const Matrix<T, 4, 4> &matrix) {
    T m_11 = matrix[0], m_12 = matrix[1], m_13 = matrix[2];
    T m_21 = matrix[4], m_22 = matrix[5], m_23 = matrix[6];
    T m_31 = matrix[8], m_32 = matrix[9], m_33 = matrix[10];

    Vector3<T> scaling;
    scaling.x = std::sqrt(m_11 * m_11 + m_12 * m_12 + m_13 * m_13);
    scaling.y = std::sqrt(m_21 * m_21 + m_22 * m_22 + m_23 * m_23);
    scaling.z = std::sqrt(m_31 * m_31 + m_32 * m_32 + m_33 * m_33);

    return scaling;
}

/**
 * Get translation from this matrix.
 * @returns Translation vector as an output parameter
 */
template <typename T>
Point3<T> getTranslation(const Matrix<T, 4, 4> &matrix) {
    return Point3<T>(matrix[12], matrix[13], matrix[14]);
}

// MARK: - Solver
/**
 * Implementation of the paper: \n
 * Matthias Müller, Jan Bender, Nuttapong Chentanez and Miles Macklin,
 * "A Robust Method to Extract the Rotational Part of Deformations",
 * ACM SIGGRAPH Motion in Games, 2016
 */
template <typename T>
void extractRotation(const Matrix3x3<T> &A, Quaternion<T> &q, unsigned int maxIter) {
    for (unsigned int iter = 0; iter < maxIter; iter++) {
        Matrix3x3<T> R = q.matrix3();
        Vector3<T> omega =
                (R.col(0).cross(A.col(0)) + R.col(1).cross(A.col(1)) + R.col(2).cross(A.col(2))) *
                (1.0 / fabs(R.col(0).dot(A.col(0)) + R.col(1).dot(A.col(1)) + R.col(2).dot(A.col(2)) + 1.0e-9));
        T w = omega.length();
        if (w < 1.0e-9) break;
        q = Quaternion<T>((1.0 / w) * omega, w) * q;
        q.normalize();
    }
}

template <typename T>
void pseudoInverse(const Matrix3x3<T> &a, Matrix3x3<T> &res) {
    const T epsilon = std::numeric_limits<T>::epsilon();
    Matrix3x3<T> u, v;
    Vector3<T> w;
    svd(a, u, w, v);
    res = v * makeScaleMatrix(1 / w.x, 1 / w.y, 1 / w.z) * u.transposed();
}

/**
 * Perform a singular value decomposition of matrix A: A = U * sigma * V^T.
 * This function returns two proper rotation matrices U and V^T which do not
 * contain a reflection. Reflections are corrected by the inversion handling
 * proposed by Irving et al. 2004.
 */
template <typename T>
void svdWithInversionHandling(const Matrix3x3<T> &A, Vector3<T> &sigma, Matrix3x3<T> &U, Matrix3x3<T> &VT) {
    Matrix3x3<T> AT_A, V;
    AT_A = A.transpose() * A;

    Vector3<T> S;

    // Eigen decomposition of A^T * A
    eigenDecomposition(AT_A, V, S);

    // Detect if V is a reflection .
    // Make a rotation out of it by multiplying one column with -1.
    const T detV = V.determinant();
    if (detV < 0.0) {
        T minLambda = std::numeric_limits<T>::max();
        unsigned char pos = 0;
        for (unsigned char l = 0; l < 3; l++) {
            if (S[l] < minLambda) {
                pos = l;
                minLambda = S[l];
            }
        }
        V(0, pos) = -V(0, pos);
        V(1, pos) = -V(1, pos);
        V(2, pos) = -V(2, pos);
    }

    if (S[0] < 0.0) S[0] = 0.0;  // safety for sqrt
    if (S[1] < 0.0) S[1] = 0.0;
    if (S[2] < 0.0) S[2] = 0.0;

    sigma[0] = sqrt(S[0]);
    sigma[1] = sqrt(S[1]);
    sigma[2] = sqrt(S[2]);

    VT = V.transpose();

    //
    // Check for values of hatF near zero
    //
    unsigned char chk = 0;
    unsigned char pos = 0;
    for (unsigned char l = 0; l < 3; l++) {
        if (fabs(sigma[l]) < 1.0e-4) {
            pos = l;
            chk++;
        }
    }

    if (chk > 0) {
        if (chk > 1) {
            U.setIdentity();
        } else {
            U = A * V;
            for (unsigned char l = 0; l < 3; l++) {
                if (l != pos) {
                    for (unsigned char m = 0; m < 3; m++) {
                        U(m, l) *= static_cast<T>(1.0) / sigma[l];
                    }
                }
            }

            Vector3<T> v[2];
            unsigned char index = 0;
            for (unsigned char l = 0; l < 3; l++) {
                if (l != pos) {
                    v[index++] = Vector3<T>(U(0, l), U(1, l), U(2, l));
                }
            }
            Vector3<T> vec = v[0].cross(v[1]);
            vec.normalize();
            U(0, pos) = vec[0];
            U(1, pos) = vec[1];
            U(2, pos) = vec[2];
        }
    } else {
        Vector3<T> sigmaInv(static_cast<T>(1.0) / sigma[0], static_cast<T>(1.0) / sigma[1],
                            static_cast<T>(1.0) / sigma[2]);
        U = A * V;
        for (unsigned char l = 0; l < 3; l++) {
            for (unsigned char m = 0; m < 3; m++) {
                U(m, l) *= sigmaInv[l];
            }
        }
    }

    const T detU = U.determinant();

    // U is a reflection => inversion
    if (detU < 0.0) {
        // std::cout << "Inversion!\n";
        T minLambda = std::numeric_limits<T>::max();
        unsigned char pos = 0;
        for (unsigned char l = 0; l < 3; l++) {
            if (sigma[l] < minLambda) {
                pos = l;
                minLambda = sigma[l];
            }
        }

        // invert values of smallest singular value
        sigma[pos] = -sigma[pos];
        U(0, pos) = -U(0, pos);
        U(1, pos) = -U(1, pos);
        U(2, pos) = -U(2, pos);
    }
}

template <typename T>
void eigenDecomposition(const Matrix3x3<T> &A, Matrix3x3<T> &eigenVecs, Vector3<T> &eigenVals) {
    const int numJacobiIterations = 10;
    const T epsilon = static_cast<T>(1e-15);

    Matrix3x3<T> D = A;

    // only for symmetric matrices!
    eigenVecs = Matrix3x3<T>::makeIdentity();  // unit matrix
    int iter = 0;
    while (iter < numJacobiIterations) {  // 3 off diagonal elements
                                          // find off diagonal element with maximum modulus
        int p, q;
        T a, max;
        max = fabs(D(0, 1));
        p = 0;
        q = 1;
        a = fabs(D(0, 2));
        if (a > max) {
            p = 0;
            q = 2;
            max = a;
        }
        a = fabs(D(1, 2));
        if (a > max) {
            p = 1;
            q = 2;
            max = a;
        }
        // all small enough -> done
        if (max < epsilon) break;
        // rotate matrix with respect to that element
        jacobiRotate(D, eigenVecs, p, q);
        iter++;
    }
    eigenVals[0] = D(0, 0);
    eigenVals[1] = D(1, 1);
    eigenVals[2] = D(2, 2);
}

template <typename T>
void jacobiRotate(Matrix3x3<T> &A, Matrix3x3<T> &R, int p, int q) {
    // rotates A through phi in pq-plane to set A(p,q) = 0
    // rotation stored in R whose columns are eigenvectors of A
    if (A(p, q) == 0.0) return;

    T d = (A(p, p) - A(q, q)) / (static_cast<T>(2.0) * A(p, q));
    T t = static_cast<T>(1.0) / (fabs(d) + sqrt(d * d + static_cast<T>(1.0)));
    if (d < 0.0) t = -t;
    T c = static_cast<T>(1.0) / sqrt(t * t + 1);
    T s = t * c;
    A(p, p) += t * A(p, q);
    A(q, q) -= t * A(p, q);
    A(p, q) = A(q, p) = 0.0;
    // transform A
    int k;
    for (k = 0; k < 3; k++) {
        if (k != p && k != q) {
            T Akp = c * A(k, p) + s * A(k, q);
            T Akq = -s * A(k, p) + c * A(k, q);
            A(k, p) = A(p, k) = Akp;
            A(k, q) = A(q, k) = Akq;
        }
    }
    // store rotation in R
    for (k = 0; k < 3; k++) {
        T Rkp = c * R(k, p) + s * R(k, q);
        T Rkq = -s * R(k, p) + c * R(k, q);
        R(k, p) = Rkp;
        R(k, q) = Rkq;
    }
}

/** Returns two orthogonal vectors to vec which are also orthogonal to each other. */
template <typename T>
void getOrthogonalVectors(const Vector3<T> &vec, Vector3<T> &x, Vector3<T> &y) {
    // Get plane vectors x, y
    Vector3<T> v(1, 0, 0);

    // Check, if v has same direction as vec
    if (fabs(v.dot(vec)) > 0.999) v = Vector3<T>(0, 1, 0);

    x = vec.cross(v);
    y = vec.cross(x);
    x.normalize();
    y.normalize();
}

/**
 * computes the APD of 8 deformation gradients. (Alg. 3 from the paper: Kugelstadt et al. "Fast Corotated FEM using
 * Operator Splitting", CGF 2018)
 */
template <typename T>
void APD_Newton(const Matrix3x3<T> &F, Quaternion<T> &q) {
    // one iteration is sufficient for plausible results
    for (int it = 0; it < 1; it++) {
        // transform quaternion to rotation matrix
        Matrix3x3<T> R;
        R = q.matrix3();

        // columns of B = RT * F
        Vector3<T> B0 = R.transpose() * F.col(0);
        Vector3<T> B1 = R.transpose() * F.col(1);
        Vector3<T> B2 = R.transpose() * F.col(2);

        Vector3<T> gradient(B2[1] - B1[2], B0[2] - B2[0], B1[0] - B0[1]);

        // compute Hessian, use the fact that it is symmetric
        const T h00 = B1[1] + B2[2];
        const T h11 = B0[0] + B2[2];
        const T h22 = B0[0] + B1[1];
        const T h01 = static_cast<T>(-0.5) * (B1[0] + B0[1]);
        const T h02 = static_cast<T>(-0.5) * (B2[0] + B0[2]);
        const T h12 = static_cast<T>(-0.5) * (B2[1] + B1[2]);

        const T detH = static_cast<T>(-1.0) * h02 * h02 * h11 + static_cast<T>(2.0) * h01 * h02 * h12 -
                       h00 * h12 * h12 - h01 * h01 * h22 + h00 * h11 * h22;

        Vector3<T> omega;
        // compute symmetric inverse
        const T factor = static_cast<T>(-0.25) / detH;
        omega[0] = (h11 * h22 - h12 * h12) * gradient[0] + (h02 * h12 - h01 * h22) * gradient[1] +
                   (h01 * h12 - h02 * h11) * gradient[2];
        omega[0] *= factor;

        omega[1] = (h02 * h12 - h01 * h22) * gradient[0] + (h00 * h22 - h02 * h02) * gradient[1] +
                   (h01 * h02 - h00 * h12) * gradient[2];
        omega[1] *= factor;

        omega[2] = (h01 * h12 - h02 * h11) * gradient[0] + (h01 * h02 - h00 * h12) * gradient[1] +
                   (h00 * h11 - h01 * h01) * gradient[2];
        omega[2] *= factor;

        // if det(H) = 0 use gradient descent, never happened in our tests, could also be removed
        if (fabs(detH) < static_cast<T>(1.0e-9)) omega = -gradient;

        // instead of clamping just use gradient descent. also works fine and does not require the norm
        if (omega.dot(gradient) > 0.0) omega = gradient * static_cast<T>(-0.125);

        const T l_omega2 = omega.squaredNorm();
        const T w = (static_cast<T>(1.0) - l_omega2) / (static_cast<T>(1.0) + l_omega2);
        const Vector3<T> vec = omega * (static_cast<T>(2.0) / (static_cast<T>(1.0) + l_omega2));
        // no normalization needed because the Cayley map returs a unit quaternion
        q = q * Quaternionr(w, vec.x(), vec.y(), vec.z());
    }
}

}  // namespace vox
