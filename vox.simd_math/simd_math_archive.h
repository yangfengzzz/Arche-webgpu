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
VOX_IO_TYPE_NOT_VERSIONABLE(math::SimdFloat4)
template <>
struct VOX_BASE_DLL Extern<math::SimdFloat4> {
    static void Save(OArchive& _archive, const math::SimdFloat4* _values, size_t _count);
    static void Load(IArchive& _archive, math::SimdFloat4* _values, size_t _count, uint32_t _version);
};

VOX_IO_TYPE_NOT_VERSIONABLE(math::SimdInt4)
template <>
struct VOX_BASE_DLL Extern<math::SimdInt4> {
    static void Save(OArchive& _archive, const math::SimdInt4* _values, size_t _count);
    static void Load(IArchive& _archive, math::SimdInt4* _values, size_t _count, uint32_t _version);
};

VOX_IO_TYPE_NOT_VERSIONABLE(math::Float4x4)
template <>
struct VOX_BASE_DLL Extern<math::Float4x4> {
    static void Save(OArchive& _archive, const math::Float4x4* _values, size_t _count);
    static void Load(IArchive& _archive, math::Float4x4* _values, size_t _count, uint32_t _version);
};
}  // namespace vox::io