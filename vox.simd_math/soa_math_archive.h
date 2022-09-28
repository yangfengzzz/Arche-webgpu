//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/io/archive_traits.h"
#include "vox.base/macros.h"

namespace vox {
namespace simd_math {
struct SoaFloat2;
struct SoaFloat3;
struct SoaFloat4;
struct SoaQuaternion;
struct SoaFloat4x4;
struct SoaTransform;
}  // namespace simd_math
namespace io {
VOX_IO_TYPE_NOT_VERSIONABLE(simd_math::SoaFloat2)
template <>
struct VOX_BASE_DLL Extern<simd_math::SoaFloat2> {
    static void Save(OArchive& _archive, const simd_math::SoaFloat2* _values, size_t _count);
    static void Load(IArchive& _archive, simd_math::SoaFloat2* _values, size_t _count, uint32_t _version);
};

VOX_IO_TYPE_NOT_VERSIONABLE(simd_math::SoaFloat3)
template <>
struct VOX_BASE_DLL Extern<simd_math::SoaFloat3> {
    static void Save(OArchive& _archive, const simd_math::SoaFloat3* _values, size_t _count);
    static void Load(IArchive& _archive, simd_math::SoaFloat3* _values, size_t _count, uint32_t _version);
};

VOX_IO_TYPE_NOT_VERSIONABLE(simd_math::SoaFloat4)
template <>
struct VOX_BASE_DLL Extern<simd_math::SoaFloat4> {
    static void Save(OArchive& _archive, const simd_math::SoaFloat4* _values, size_t _count);
    static void Load(IArchive& _archive, simd_math::SoaFloat4* _values, size_t _count, uint32_t _version);
};

VOX_IO_TYPE_NOT_VERSIONABLE(simd_math::SoaQuaternion)
template <>
struct VOX_BASE_DLL Extern<simd_math::SoaQuaternion> {
    static void Save(OArchive& _archive, const simd_math::SoaQuaternion* _values, size_t _count);
    static void Load(IArchive& _archive, simd_math::SoaQuaternion* _values, size_t _count, uint32_t _version);
};

VOX_IO_TYPE_NOT_VERSIONABLE(simd_math::SoaFloat4x4)
template <>
struct VOX_BASE_DLL Extern<simd_math::SoaFloat4x4> {
    static void Save(OArchive& _archive, const simd_math::SoaFloat4x4* _values, size_t _count);
    static void Load(IArchive& _archive, simd_math::SoaFloat4x4* _values, size_t _count, uint32_t _version);
};

VOX_IO_TYPE_NOT_VERSIONABLE(simd_math::SoaTransform)
template <>
struct VOX_BASE_DLL Extern<simd_math::SoaTransform> {
    static void Save(OArchive& _archive, const simd_math::SoaTransform* _values, size_t _count);
    static void Load(IArchive& _archive, simd_math::SoaTransform* _values, size_t _count, uint32_t _version);
};
}  // namespace io
}  // namespace vox