//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cassert>

#include "vox.base/macros.h"
#include "vox.simd_math/soa_float.h"

namespace vox::math {

struct SoaQuaternion {
    SimdFloat4 x, y, z, w;

    // Loads a quaternion from 4 SimdFloat4 values.
    static VOX_INLINE SoaQuaternion Load(_SimdFloat4 _x, _SimdFloat4 _y, _SimdFloat4 _z, const SimdFloat4& _w) {
        const SoaQuaternion r = {_x, _y, _z, _w};
        return r;
    }

    // Returns the identity SoaQuaternion.
    static VOX_INLINE SoaQuaternion identity() {
        const SimdFloat4 zero = simd_float4::zero();
        const SoaQuaternion r = {zero, zero, zero, simd_float4::one()};
        return r;
    }
};

// Returns the conjugate of _q. This is the same as the inverse if _q is
// normalized. Otherwise, the magnitude of the inverse is 1.f/|_q|.
VOX_INLINE SoaQuaternion Conjugate(const SoaQuaternion& _q) {
    const SoaQuaternion r = {-_q.x, -_q.y, -_q.z, _q.w};
    return r;
}

// Returns to negate of _q. This represents the same rotation as q.
VOX_INLINE SoaQuaternion operator-(const SoaQuaternion& _q) {
    const SoaQuaternion r = {-_q.x, -_q.y, -_q.z, -_q.w};
    return r;
}

// Returns the 4D dot product of quaternion _a and _b.
VOX_INLINE SimdFloat4 Dot(const SoaQuaternion& _a, const SoaQuaternion& _b) {
    return _a.x * _b.x + _a.y * _b.y + _a.z * _b.z + _a.w * _b.w;
}

// Returns the normalized SoaQuaternion _q.
VOX_INLINE SoaQuaternion Normalize(const SoaQuaternion& _q) {
    const SimdFloat4 len2 = _q.x * _q.x + _q.y * _q.y + _q.z * _q.z + _q.w * _q.w;
    const SimdFloat4 inv_len = math::simd_float4::one() / Sqrt(len2);
    const SoaQuaternion r = {_q.x * inv_len, _q.y * inv_len, _q.z * inv_len, _q.w * inv_len};
    return r;
}

// Returns the estimated normalized SoaQuaternion _q.
VOX_INLINE SoaQuaternion NormalizeEst(const SoaQuaternion& _q) {
    const SimdFloat4 len2 = _q.x * _q.x + _q.y * _q.y + _q.z * _q.z + _q.w * _q.w;
    // Uses RSqrtEstNR (with one more Newton-Raphson step) as quaternions loose
    // much precision due to normalization.
    const SimdFloat4 inv_len = RSqrtEstNR(len2);
    const SoaQuaternion r = {_q.x * inv_len, _q.y * inv_len, _q.z * inv_len, _q.w * inv_len};
    return r;
}

// Test if each quaternion of _q is normalized.
VOX_INLINE SimdInt4 IsNormalized(const SoaQuaternion& _q) {
    const SimdFloat4 len2 = _q.x * _q.x + _q.y * _q.y + _q.z * _q.z + _q.w * _q.w;
    return CmpLt(Abs(len2 - math::simd_float4::one()), simd_float4::Load1(kNormalizationToleranceSq));
}

// Test if each quaternion of _q is normalized. using estimated tolerance.
VOX_INLINE SimdInt4 IsNormalizedEst(const SoaQuaternion& _q) {
    const SimdFloat4 len2 = _q.x * _q.x + _q.y * _q.y + _q.z * _q.z + _q.w * _q.w;
    return CmpLt(Abs(len2 - math::simd_float4::one()), simd_float4::Load1(kNormalizationToleranceEstSq));
}

// Returns the linear interpolation of SoaQuaternion _a and _b with coefficient
// _f.
VOX_INLINE SoaQuaternion Lerp(const SoaQuaternion& _a, const SoaQuaternion& _b, _SimdFloat4 _f) {
    const SoaQuaternion r = {(_b.x - _a.x) * _f + _a.x, (_b.y - _a.y) * _f + _a.y, (_b.z - _a.z) * _f + _a.z,
                             (_b.w - _a.w) * _f + _a.w};
    return r;
}

// Returns the linear interpolation of SoaQuaternion _a and _b with coefficient
// _f.
VOX_INLINE SoaQuaternion NLerp(const SoaQuaternion& _a, const SoaQuaternion& _b, _SimdFloat4 _f) {
    const SoaFloat4 lerp = {(_b.x - _a.x) * _f + _a.x, (_b.y - _a.y) * _f + _a.y, (_b.z - _a.z) * _f + _a.z,
                            (_b.w - _a.w) * _f + _a.w};
    const SimdFloat4 len2 = lerp.x * lerp.x + lerp.y * lerp.y + lerp.z * lerp.z + lerp.w * lerp.w;
    const SimdFloat4 inv_len = math::simd_float4::one() / Sqrt(len2);
    const SoaQuaternion r = {lerp.x * inv_len, lerp.y * inv_len, lerp.z * inv_len, lerp.w * inv_len};
    return r;
}

// Returns the estimated linear interpolation of SoaQuaternion _a and _b with
// coefficient _f.
VOX_INLINE SoaQuaternion NLerpEst(const SoaQuaternion& _a, const SoaQuaternion& _b, _SimdFloat4 _f) {
    const SoaFloat4 lerp = {(_b.x - _a.x) * _f + _a.x, (_b.y - _a.y) * _f + _a.y, (_b.z - _a.z) * _f + _a.z,
                            (_b.w - _a.w) * _f + _a.w};
    const SimdFloat4 len2 = lerp.x * lerp.x + lerp.y * lerp.y + lerp.z * lerp.z + lerp.w * lerp.w;
    // Uses RSqrtEstNR (with one more Newton-Raphson step) as quaternions loose
    // much precision due to normalization.
    const SimdFloat4 inv_len = RSqrtEstNR(len2);
    const SoaQuaternion r = {lerp.x * inv_len, lerp.y * inv_len, lerp.z * inv_len, lerp.w * inv_len};
    return r;
}
}  // namespace vox::math

// Returns the addition of _a and _b.
VOX_INLINE vox::math::SoaQuaternion operator+(const vox::math::SoaQuaternion& _a, const vox::math::SoaQuaternion& _b) {
    const vox::math::SoaQuaternion r = {_a.x + _b.x, _a.y + _b.y, _a.z + _b.z, _a.w + _b.w};
    return r;
}

// Returns the multiplication of _q and scalar value _f.
VOX_INLINE vox::math::SoaQuaternion operator*(const vox::math::SoaQuaternion& _q, const vox::math::SimdFloat4& _f) {
    const vox::math::SoaQuaternion r = {_q.x * _f, _q.y * _f, _q.z * _f, _q.w * _f};
    return r;
}

// Returns the multiplication of _a and _b. If both _a and _b are normalized,
// then the result is normalized.
VOX_INLINE vox::math::SoaQuaternion operator*(const vox::math::SoaQuaternion& _a, const vox::math::SoaQuaternion& _b) {
    const vox::math::SoaQuaternion r = {_a.w * _b.x + _a.x * _b.w + _a.y * _b.z - _a.z * _b.y,
                                        _a.w * _b.y + _a.y * _b.w + _a.z * _b.x - _a.x * _b.z,
                                        _a.w * _b.z + _a.z * _b.w + _a.x * _b.y - _a.y * _b.x,
                                        _a.w * _b.w - _a.x * _b.x - _a.y * _b.y - _a.z * _b.z};
    return r;
}

// Returns true if each element of _a is equal to each element of _b.
// Uses a bitwise comparison of _a and _b, no tolerance is applied.
VOX_INLINE vox::math::SimdInt4 operator==(const vox::math::SoaQuaternion& _a, const vox::math::SoaQuaternion& _b) {
    const vox::math::SimdInt4 x = vox::math::CmpEq(_a.x, _b.x);
    const vox::math::SimdInt4 y = vox::math::CmpEq(_a.y, _b.y);
    const vox::math::SimdInt4 z = vox::math::CmpEq(_a.z, _b.z);
    const vox::math::SimdInt4 w = vox::math::CmpEq(_a.w, _b.w);
    return vox::math::And(vox::math::And(vox::math::And(x, y), z), w);
}