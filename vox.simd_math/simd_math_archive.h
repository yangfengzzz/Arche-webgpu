//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/io/archive_traits.h"
#include "vox.base/macros.h"
#include "vox.simd_math/simd_math.h"

namespace vox::io {
VOX_IO_TYPE_NOT_VERSIONABLE(simd_math::SimdFloat4)
template <>
struct VOX_BASE_DLL Extern<simd_math::SimdFloat4> {
    static void Save(OArchive& _archive, const simd_math::SimdFloat4* _values, size_t _count);
    static void Load(IArchive& _archive, simd_math::SimdFloat4* _values, size_t _count, uint32_t _version);
};

VOX_IO_TYPE_NOT_VERSIONABLE(simd_math::SimdInt4)
template <>
struct VOX_BASE_DLL Extern<simd_math::SimdInt4> {
    static void Save(OArchive& _archive, const simd_math::SimdInt4* _values, size_t _count);
    static void Load(IArchive& _archive, simd_math::SimdInt4* _values, size_t _count, uint32_t _version);
};

VOX_IO_TYPE_NOT_VERSIONABLE(simd_math::Float4x4)
template <>
struct VOX_BASE_DLL Extern<simd_math::Float4x4> {
    static void Save(OArchive& _archive, const simd_math::Float4x4* _values, size_t _count);
    static void Load(IArchive& _archive, simd_math::Float4x4* _values, size_t _count, uint32_t _version);
};
}  // namespace vox::io