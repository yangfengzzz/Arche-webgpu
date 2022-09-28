//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/io/archive_traits.h"
#include "vox.base/macros.h"
#include "vox.math/bounding_box3.h"
#include "vox.math/quaternion.h"
#include "vox.math/rect.h"
#include "vox.math/scalable_transform.h"
#include "vox.math/vector2.h"
#include "vox.math/vector3.h"
#include "vox.math/vector4.h"

namespace vox::io {
VOX_IO_TYPE_NOT_VERSIONABLE(Vector2F)
template <>
struct VOX_BASE_DLL Extern<Vector2F> {
    static void Save(OArchive& _archive, const Vector2F* _values, size_t _count);
    static void Load(IArchive& _archive, Vector2F* _values, size_t _count, uint32_t _version);
};

VOX_IO_TYPE_NOT_VERSIONABLE(Vector3F)
template <>
struct VOX_BASE_DLL Extern<Vector3F> {
    static void Save(OArchive& _archive, const Vector3F* _values, size_t _count);
    static void Load(IArchive& _archive, Vector3F* _values, size_t _count, uint32_t _version);
};

VOX_IO_TYPE_NOT_VERSIONABLE(Vector4F)
template <>
struct VOX_BASE_DLL Extern<Vector4F> {
    static void Save(OArchive& _archive, const Vector4F* _values, size_t _count);
    static void Load(IArchive& _archive, Vector4F* _values, size_t _count, uint32_t _version);
};

VOX_IO_TYPE_NOT_VERSIONABLE(QuaternionF)
template <>
struct VOX_BASE_DLL Extern<QuaternionF> {
    static void Save(OArchive& _archive, const QuaternionF* _values, size_t _count);
    static void Load(IArchive& _archive, QuaternionF* _values, size_t _count, uint32_t _version);
};

VOX_IO_TYPE_NOT_VERSIONABLE(ScalableTransform)
template <>
struct VOX_BASE_DLL Extern<ScalableTransform> {
    static void Save(OArchive& _archive, const ScalableTransform* _values, size_t _count);
    static void Load(IArchive& _archive, ScalableTransform* _values, size_t _count, uint32_t _version);
};

VOX_IO_TYPE_NOT_VERSIONABLE(BoundingBox3F)
template <>
struct VOX_BASE_DLL Extern<BoundingBox3F> {
    static void Save(OArchive& _archive, const BoundingBox3F* _values, size_t _count);
    static void Load(IArchive& _archive, BoundingBox3F* _values, size_t _count, uint32_t _version);
};

VOX_IO_TYPE_NOT_VERSIONABLE(RectFloat)
template <>
struct VOX_BASE_DLL Extern<RectFloat> {
    static void Save(OArchive& _archive, const RectFloat* _values, size_t _count);
    static void Load(IArchive& _archive, RectFloat* _values, size_t _count, uint32_t _version);
};

VOX_IO_TYPE_NOT_VERSIONABLE(RectInt)
template <>
struct VOX_BASE_DLL Extern<RectInt> {
    static void Save(OArchive& _archive, const RectInt* _values, size_t _count);
    static void Load(IArchive& _archive, RectInt* _values, size_t _count, uint32_t _version);
};
}  // namespace vox::io
