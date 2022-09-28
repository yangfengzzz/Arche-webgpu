//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cstddef>
#include <limits>

#include "vox.base/macros.h"

namespace vox {

//!
//! \brief      Returns true if \p x and \p y are similar.
//!
//! \param[in]  x     The first value.
//! \param[in]  y     The second value.
//! \param[in]  eps   The tolerance.
//!
//! \tparam     T     Value type.
//!
//! \return     True if similar.
//!
template <typename T>
VOX_INLINE bool similar(T x, T y, T eps = std::numeric_limits<T>::epsilon());

//!
//! \brief      Returns the sign of the value.
//!
//! \param[in]  x     Input value.
//!
//! \tparam     T     Value type.
//!
//! \return     The sign.
//!
template <typename T>
VOX_INLINE T sign(T x);

//!
//! \brief      Returns the minimum value among three inputs.
//!
//! \param[in]  x     The first value.
//! \param[in]  y     The second value.
//! \param[in]  z     The three value.
//!
//! \tparam     T     Value type.
//!
//! \return     The minimum value.
//!
template <typename T>
VOX_INLINE T min3(T x, T y, T z);

//!
//! \brief      Returns the maximum value among three inputs.
//!
//! \param[in]  x     The first value.
//! \param[in]  y     The second value.
//! \param[in]  z     The three value.
//!
//! \tparam     T     Value type.
//!
//! \return     The maximum value.
//!
template <typename T>
VOX_INLINE T max3(T x, T y, T z);

//! Returns minimum among n-elements.
template <typename T>
VOX_INLINE T minn(const T *x, size_t n);

//! Returns maximum among n-elements.
template <typename T>
VOX_INLINE T maxn(const T *x, size_t n);

//!
//! \brief      Returns the absolute minimum value among the two inputs.
//!
//! \param[in]  x     The first value.
//! \param[in]  y     The second value.
//!
//! \tparam     T     Value type.
//!
//! \return     The absolute minimum.
//!
template <typename T>
VOX_INLINE T absmin(T x, T y);

//!
//! \brief      Returns the absolute maximum value among the two inputs.
//!
//! \param[in]  x     The first value.
//! \param[in]  y     The second value.
//!
//! \tparam     T     Value type.
//!
//! \return     The absolute maximum.
//!
template <typename T>
VOX_INLINE T absmax(T x, T y);

//! Returns absolute minimum among n-elements.
template <typename T>
VOX_INLINE T absminn(const T *x, size_t n);

//! Returns absolute maximum among n-elements.
template <typename T>
VOX_INLINE T absmaxn(const T *x, size_t n);

template <typename T>
VOX_INLINE size_t argmin2(T x, T y);

template <typename T>
VOX_INLINE size_t argmax2(T x, T y);

template <typename T>
VOX_INLINE size_t argmin3(T x, T y, T z);

template <typename T>
VOX_INLINE size_t argmax3(T x, T y, T z);

//!
//! \brief      Returns the square of \p x.
//!
//! \param[in]  x     The input.
//!
//! \tparam     T     Value type.
//!
//! \return     The squared value.
//!
template <typename T>
VOX_INLINE T square(T x);

//!
//! \brief      Returns the cubic of \p x.
//!
//! \param[in]  x     The input.
//!
//! \tparam     T     Value type.
//!
//! \return     The cubic of \p x.
//!
template <typename T>
VOX_INLINE T cubic(T x);

//!
//! \brief      Returns the clamped value.
//!
//! \param[in]  val   The value.
//! \param[in]  low   The low value.
//! \param[in]  high  The high value.
//!
//! \tparam     T     Value type.
//!
//! \return     The clamped value.
//!
template <typename T>
VOX_INLINE T clamp(T val, T low, T high);

//!
//! \brief      Converts degrees to radians.
//!
//! \param[in]  angle_in_degrees The angle in degrees.
//!
//! \tparam     T              Value type.
//!
//! \return     Angle in radians.
//!
template <typename T>
VOX_INLINE T degreesToRadians(T angle_in_degrees);

//!
//! \brief      Converts radians to degrees.
//!
//! \param[in]  angle_in_radians The angle in radians.
//!
//! \tparam     T              Value type.
//!
//! \return     Angle in degrees.
//!
template <typename T>
VOX_INLINE T radiansToDegrees(T angle_in_radians);

//!
//! \brief      Gets the barycentric coordinate.
//!
//! \param[in]  x     The input value.
//! \param[in]  i_low  The lowest index.
//! \param[in]  i_high The highest index.
//! \param      i     The output index.
//! \param      f     The offset from \p i.
//!
//! \tparam     T     Value type.
//!
template <class T>
VOX_INLINE void getBarycentric(T x, ssize_t i_low, ssize_t i_high, ssize_t *i, T *f);

//!
//! \brief      Computes linear interpolation.
//!
//! \param[in]  value_0    The first value.
//! \param[in]  value_1    The second value.
//! \param[in]  f     Relative offset [0, 1] from the first value.
//!
//! \tparam     S     Input value type.
//! \tparam     T     Offset type.
//!
//! \return     The interpolated value.
//!
template <typename S, typename T>
VOX_INLINE S lerp(const S &value_0, const S &value_1, T f);

//! \brief      Computes bilinear interpolation.
template <typename S, typename T>
VOX_INLINE S bilerp(const S &f_00, const S &f_10, const S &f_01, const S &f_11, T tx, T ty);

//! \brief      Computes trilinear interpolation.
template <typename S, typename T>
VOX_INLINE S trilerp(const S &f_000,
                     const S &f_100,
                     const S &f010,
                     const S &f110,
                     const S &f001,
                     const S &f101,
                     const S &f011,
                     const S &f111,
                     T tx,
                     T ty,
                     T fz);

//! \brief      Computes Catmull-Rom interpolation.
template <typename S, typename T>
VOX_INLINE S catmullRom(const S &f0, const S &f1, const S &f2, const S &f3, T f);

//! \brief      Computes monotonic Catmull-Rom interpolation.
template <typename T>
VOX_INLINE T monotonicCatmullRom(const T &f0, const T &f1, const T &f2, const T &f3, T f);

// Implements int selection, avoiding branching.
VOX_INLINE int select(bool _b, int _true, int _false) { return _false ^ (-static_cast<int>(_b) & (_true ^ _false)); }

// Implements float selection, avoiding branching.
VOX_INLINE float select(bool _b, float _true, float _false) {
    union {
        float f;
        int32_t i;
    } t = {_true};
    union {
        float f;
        int32_t i;
    } f = {_false};
    union {
        int32_t i;
        float f;
    } r = {f.i ^ (-static_cast<int32_t>(_b) & (t.i ^ f.i))};
    return r.f;
}

// Implements pointer selection, avoiding branching.
template <typename Ty>
VOX_INLINE Ty *select(bool _b, Ty *_true, Ty *_false) {
    union {
        Ty *p;
        intptr_t i;
    } t = {_true};
    union {
        Ty *p;
        intptr_t i;
    } f = {_false};
    union {
        intptr_t i;
        Ty *p;
    } r = {f.i ^ (-static_cast<intptr_t>(_b) & (t.i ^ f.i))};
    return r.p;
}

// Implements const pointer selection, avoiding branching.
template <typename Ty>
VOX_INLINE const Ty *select(bool _b, const Ty *_true, const Ty *_false) {
    union {
        const Ty *p;
        intptr_t i;
    } t = {_true};
    union {
        const Ty *p;
        intptr_t i;
    } f = {_false};
    union {
        intptr_t i;
        const Ty *p;
    } r = {f.i ^ (-static_cast<intptr_t>(_b) & (t.i ^ f.i))};
    return r.p;
}

}  // namespace vox

#include "vox.math/math_utils-inl.h"
