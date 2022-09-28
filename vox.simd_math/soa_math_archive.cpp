//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.simd_math/soa_math_archive.h"

#include "vox.base/io/archive.h"
#include "vox.simd_math/soa_float.h"
#include "vox.simd_math/soa_float4x4.h"
#include "vox.simd_math/soa_transform.h"

namespace vox::io {
void Extern<math::SoaFloat2>::Save(OArchive& _archive, const math::SoaFloat2* _values, size_t _count) {
    _archive << MakeArray(reinterpret_cast<const float*>(&_values->x), 2 * 4 * _count);
}
void Extern<math::SoaFloat2>::Load(IArchive& _archive, math::SoaFloat2* _values, size_t _count, uint32_t _version) {
    (void)_version;
    _archive >> MakeArray(reinterpret_cast<float*>(&_values->x), 2 * 4 * _count);
}

void Extern<math::SoaFloat3>::Save(OArchive& _archive, const math::SoaFloat3* _values, size_t _count) {
    _archive << MakeArray(reinterpret_cast<const float*>(&_values->x), 3 * 4 * _count);
}
void Extern<math::SoaFloat3>::Load(IArchive& _archive, math::SoaFloat3* _values, size_t _count, uint32_t _version) {
    (void)_version;
    _archive >> MakeArray(reinterpret_cast<float*>(&_values->x), 3 * 4 * _count);
}

void Extern<math::SoaFloat4>::Save(OArchive& _archive, const math::SoaFloat4* _values, size_t _count) {
    _archive << MakeArray(reinterpret_cast<const float*>(&_values->x), 4 * 4 * _count);
}
void Extern<math::SoaFloat4>::Load(IArchive& _archive, math::SoaFloat4* _values, size_t _count, uint32_t _version) {
    (void)_version;
    _archive >> MakeArray(reinterpret_cast<float*>(&_values->x), 4 * 4 * _count);
}

void Extern<math::SoaQuaternion>::Save(OArchive& _archive, const math::SoaQuaternion* _values, size_t _count) {
    _archive << MakeArray(reinterpret_cast<const float*>(&_values->x), 4 * 4 * _count);
}
void Extern<math::SoaQuaternion>::Load(IArchive& _archive,
                                       math::SoaQuaternion* _values,
                                       size_t _count,
                                       uint32_t _version) {
    (void)_version;
    _archive >> MakeArray(reinterpret_cast<float*>(&_values->x), 4 * 4 * _count);
}

void Extern<math::SoaFloat4x4>::Save(OArchive& _archive, const math::SoaFloat4x4* _values, size_t _count) {
    _archive << MakeArray(reinterpret_cast<const float*>(&_values->cols[0].x), 4 * 4 * 4 * _count);
}
void Extern<math::SoaFloat4x4>::Load(IArchive& _archive, math::SoaFloat4x4* _values, size_t _count, uint32_t _version) {
    (void)_version;
    _archive >> MakeArray(reinterpret_cast<float*>(&_values->cols[0].x), 4 * 4 * 4 * _count);
}

void Extern<math::SoaTransform>::Save(OArchive& _archive, const math::SoaTransform* _values, size_t _count) {
    _archive << MakeArray(reinterpret_cast<const float*>(&_values->translation.x), 10 * 4 * _count);
}
void Extern<math::SoaTransform>::Load(IArchive& _archive,
                                      math::SoaTransform* _values,
                                      size_t _count,
                                      uint32_t _version) {
    (void)_version;
    _archive >> MakeArray(reinterpret_cast<float*>(&_values->translation.x), 10 * 4 * _count);
}
}  // namespace vox::io
