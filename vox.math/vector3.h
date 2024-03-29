//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>
#include <limits>
#include <tuple>

#include "vox.math/vector2.h"

namespace vox {

//!
//! \brief 3-D vector class.
//!
//! This class defines simple 3-D vector data.
//!
//! \tparam T - Type of the element
//!
template <typename T>
struct Vector<T, 3> final {
public:
    //! X (or the first) component of the vector.
    T x;

    //! Y (or the second) component of the vector.
    T y;

    //! Z (or the third) component of the vector.
    T z;

    // MARK: - Constructors

    //! Constructs default vector (0, 0, 0).
    constexpr Vector() : x(0), y(0), z(0) {}

    //! Constructs vector with given parameters \p x_.
    constexpr explicit Vector(T x) : x(x), y(x), z(x) {}

    //! Constructs vector with given parameters \p x_, \p y_, and \p z_.
    constexpr Vector(T x, T y, T z) : x(x), y(y), z(z) {}

    //! Constructs vector with a 2-D vector and a scalar.
    constexpr Vector(const Vector2<T> &v, T z) : x(v.x), y(v.y), z(z) {}

    //! Constructs vector with initializer list.
    template <typename U>
    Vector(const std::initializer_list<U> &lst);

    //! Copy constructor.
    constexpr Vector(const Vector &v) : x(v.x), y(v.y), z(v.z) {}

    // MARK: - Basic setters

    //! Set all x, y, and z components to \p s.
    void set(T s);

    //! Set x, y, and z components with given parameters.
    void set(T new_x, T new_y, T new_z);

    //! Set x, y, and z components with given \p pt.x, \p pt.y, and \p z.
    void set(const Vector2<T> &pt, T new_z);

    //! Set x, y, and z components with given initializer list.
    template <typename U>
    void set(const std::initializer_list<U> &lst);

    //! Set x, y, and z with other vector \p v.
    void set(const Vector &v);

    //! Set all x, y, and z to zero.
    void setZero();

    //! Normalizes this vector.
    void normalize();

    // MARK: - Binary operations: new instance = this (+) v

    //! Computes this + (v, v, v).
    Vector add(T v) const;

    //! Computes this + (v.x, v.y, v.z).
    Vector add(const Vector &v) const;

    //! Computes this - (v, v, v).
    Vector sub(T v) const;

    //! Computes this - (v.x, v.y, v.z).
    Vector sub(const Vector &v) const;

    //! Computes this * (v, v, v).
    Vector mul(T v) const;

    //! Computes this * (v.x, v.y, v.z).
    Vector mul(const Vector &v) const;

    //! Computes this / (v, v, v).
    Vector div(T v) const;

    //! Computes this / (v.x, v.y, v.z).
    Vector div(const Vector &v) const;

    //! Computes dot product.
    T dot(const Vector &v) const;

    //! Computes cross product.
    Vector cross(const Vector &v) const;

    // MARK: - Binary operations: new instance = v (+) this

    //! Computes (v, v, v) - this.
    Vector rsub(T v) const;

    //! Computes (v.x, v.y, v.z) - this.
    Vector rsub(const Vector &v) const;

    //! Computes (v, v, v) / this.
    Vector rdiv(T v) const;

    //! Computes (v.x, v.y, v.z) / this.
    Vector rdiv(const Vector &v) const;

    //! Computes \p v cross this.
    Vector rcross(const Vector &v) const;

    // MARK: - Augmented operations: this (+)= v

    //! Computes this += (v, v, v).
    void iadd(T v);

    //! Computes this += (v.x, v.y, v.z).
    void iadd(const Vector &v);

    //! Computes this -= (v, v, v).
    void isub(T v);

    //! Computes this -= (v.x, v.y, v.z).
    void isub(const Vector &v);

    //! Computes this *= (v, v, v).
    void imul(T v);

    //! Computes this *= (v.x, v.y, v.z).
    void imul(const Vector &v);

    //! Computes this /= (v, v, v).
    void idiv(T v);

    //! Computes this /= (v.x, v.y, v.z).
    void idiv(const Vector &v);

    // MARK: - Basic getters

    //! Returns const reference to the \p i -th element of the vector.
    const T &at(size_t i) const;

    //! Returns reference to the \p i -th element of the vector.
    T &at(size_t i);

    //! Returns the sum of all the components (i.e. x + y + z).
    T sum() const;

    //! Returns the average of all the components.
    T avg() const;

    //! Returns the minimum value among x, y, and z.
    T min() const;

    //! Returns the maximum value among x, y, and z.
    T max() const;

    //! Returns the absolute minimum value among x, y, and z.
    T absmin() const;

    //! Returns the absolute maximum value among x, y, and z.
    T absmax() const;

    //! Returns the index of the dominant axis.
    [[nodiscard]] size_t dominantAxis() const;

    //! Returns the index of the subminant axis.
    [[nodiscard]] size_t subminantAxis() const;

    //! Returns normalized vector.
    Vector normalized() const;

    //! Returns the length of the vector.
    T length() const;

    //! Returns the squared length of the vector.
    T lengthSquared() const;

    //! Returns the distance to the other vector.
    T distanceTo(const Vector &other) const;

    //! Returns the squared distance to the other vector.
    T distanceSquaredTo(const Vector &other) const;

    //! Returns the tangential vector for this vector.
    std::tuple<Vector, Vector> tangential() const;

    //! Returns a vector with different value type.
    template <typename U>
    Vector<U, 3> castTo() const;

    //! Returns true if \p other is the same as this vector.
    bool isEqual(const Vector &other) const;

    //! Returns true if \p other is similar to this vector.
    bool isSimilar(const Vector &other, T epsilon = std::numeric_limits<T>::epsilon()) const;

    // MARK: - Operators

    //! Returns reference to the \p i -th element of the vector.
    T &operator[](size_t i);

    //! Returns const reference to the \p i -th element of the vector.
    const T &operator[](size_t i) const;

    //! Set x and y components with given initializer list.
    template <typename U>
    Vector &operator=(const std::initializer_list<U> &lst);

    //! Set x and y with other vector \p pt.
    Vector &operator=(const Vector &v);

    //! Computes this += (v, v, v)
    Vector &operator+=(T v);

    //! Computes this += (v.x, v.y, v.z)
    Vector &operator+=(const Vector &v);

    //! Computes this -= (v, v, v)
    Vector &operator-=(T v);

    //! Computes this -= (v.x, v.y, v.z)
    Vector &operator-=(const Vector &v);

    //! Computes this *= (v, v, v)
    Vector &operator*=(T v);

    //! Computes this *= (v.x, v.y, v.z)
    Vector &operator*=(const Vector &v);

    //! Computes this /= (v, v, v)
    Vector &operator/=(T v);

    //! Computes this /= (v.x, v.y, v.z)
    Vector &operator/=(const Vector &v);

    //! Returns true if \p other is the same as this vector.
    bool operator==(const Vector &v) const;

    //! Returns true if \p other is the not same as this vector.
    bool operator!=(const Vector &v) const;

    // Returns a vector with all components set to 0.
    static VOX_INLINE Vector zero() { return Vector(0.f); }

    // Returns a vector with all components set to 1.
    static VOX_INLINE Vector one() { return Vector(1.f); }

    // Returns a unitary vector x.
    static VOX_INLINE Vector x_axis() { return Vector(1.f, 0.f, 0.f); }

    // Returns a unitary vector y.
    static VOX_INLINE Vector y_axis() { return Vector(0.f, 1.f, 0.f); }

    // Returns a unitary vector z.
    static VOX_INLINE Vector z_axis() { return Vector(0.f, 0.f, 1.f); }
};

//! Type alias for three dimensional vector.
template <typename T>
using Vector3 = Vector<T, 3>;

//! Positive sign operator.
template <typename T>
Vector3<T> operator+(const Vector3<T> &a);

//! Negative sign operator.
template <typename T>
Vector3<T> operator-(const Vector3<T> &a);

//! Computes (a, a, a) + (b.x, b.y, b.z).
template <typename T>
Vector3<T> operator+(T a, const Vector3<T> &b);

//! Computes (a.x, a.y, a.z) + (b.x, b.y, b.z).
template <typename T>
Vector3<T> operator+(const Vector3<T> &a, const Vector3<T> &b);

//! Computes (a.x, a.y, a.z) - (b, b, b).
template <typename T>
Vector3<T> operator-(const Vector3<T> &a, T b);

//! Computes (a, a, a) - (b.x, b.y, b.z).
template <typename T>
Vector3<T> operator-(T a, const Vector3<T> &b);

//! Computes (a.x, a.y, a.z) - (b.x, b.y, b.z).
template <typename T>
Vector3<T> operator-(const Vector3<T> &a, const Vector3<T> &b);

//! Computes (a.x, a.y, a.z) * (b, b, b).
template <typename T>
Vector3<T> operator*(const Vector3<T> &a, T b);

//! Computes (a, a, a) * (b.x, b.y, b.z).
template <typename T>
Vector3<T> operator*(T a, const Vector3<T> &b);

//! Computes (a.x, a.y, a.z) * (b.x, b.y, b.z).
template <typename T>
Vector3<T> operator*(const Vector3<T> &a, const Vector3<T> &b);

//! Computes (a.x, a.y, a.z) / (b, b, b).
template <typename T>
Vector3<T> operator/(const Vector3<T> &a, T b);

//! Computes (a, a, a) / (b.x, b.y, b.z).
template <typename T>
Vector3<T> operator/(T a, const Vector3<T> &b);

//! Computes (a.x, a.y, a.z) / (b.x, b.y, b.z).
template <typename T>
Vector3<T> operator/(const Vector3<T> &a, const Vector3<T> &b);

//! Returns element-wise min vector.
template <typename T>
Vector3<T> min(const Vector3<T> &a, const Vector3<T> &b);

//! Returns element-wise max vector.
template <typename T>
Vector3<T> max(const Vector3<T> &a, const Vector3<T> &b);

//! Returns element-wise clamped vector.
template <typename T>
Vector3<T> clamp(const Vector3<T> &v, const Vector3<T> &low, const Vector3<T> &high);

//! Returns element-wise ceiled vector.
template <typename T>
Vector3<T> ceil(const Vector3<T> &a);

//! Returns element-wise floored vector.
template <typename T>
Vector3<T> floor(const Vector3<T> &a);

//! Float-type 3D vector.
using Vector3F = Vector3<float>;

//! Double-type 3D vector.
using Vector3D = Vector3<double>;

// MARK: - Extensions

//! Returns float-type zero vector.
template <>
constexpr Vector3F zero<Vector3F>() {
    return Vector3F(0.f, 0.f, 0.f);
}

//! Returns double-type zero vector.
template <>
constexpr Vector3D zero<Vector3D>() {
    return Vector3D(0.0, 0.0, 0.0);
}

//! Returns the type of the value itself.
template <typename T>
struct ScalarType<Vector3<T>> {
    using value = T;
};

//! Computes monotonic Catmull-Rom interpolation.
template <typename T>
Vector3<T> monotonicCatmullRom(
        const Vector3<T> &v_0, const Vector3<T> &v_1, const Vector3<T> &v_2, const Vector3<T> &v_3, T f);

}  // namespace vox

#include "vox.math/vector3-inl.h"
