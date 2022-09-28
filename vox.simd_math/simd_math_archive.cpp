//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.simd_math/simd_math_archive.h"

#include <cassert>

#include "vox.base/io/archive.h"

namespace vox::io {
void Extern<simd_math::SimdFloat4>::Save(OArchive& _archive, const simd_math::SimdFloat4* _values, size_t _count) {
    _archive << MakeArray(reinterpret_cast<const float*>(_values), 4 * _count);
}
void Extern<simd_math::SimdFloat4>::Load(IArchive& _archive, simd_math::SimdFloat4* _values, size_t _count, uint32_t _version) {
    (void)_version;
    _archive >> MakeArray(reinterpret_cast<float*>(_values), 4 * _count);
}

void Extern<simd_math::SimdInt4>::Save(OArchive& _archive, const simd_math::SimdInt4* _values, size_t _count) {
    _archive << MakeArray(reinterpret_cast<const int*>(_values), 4 * _count);
}
void Extern<simd_math::SimdInt4>::Load(IArchive& _archive, simd_math::SimdInt4* _values, size_t _count, uint32_t _version) {
    (void)_version;
    _archive >> MakeArray(reinterpret_cast<int*>(_values), 4 * _count);
}

void Extern<simd_math::Float4x4>::Save(OArchive& _archive, const simd_math::Float4x4* _values, size_t _count) {
    _archive << MakeArray(reinterpret_cast<const float*>(_values), 16 * _count);
}
void Extern<simd_math::Float4x4>::Load(IArchive& _archive, simd_math::Float4x4* _values, size_t _count, uint32_t _version) {
    (void)_version;
    _archive >> MakeArray(reinterpret_cast<float*>(_values), 16 * _count);
}
}  // namespace vox::io
