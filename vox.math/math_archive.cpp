//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.math/math_archive.h"

#include <cassert>

#include "vox.base/io/archive.h"

namespace vox::io {
void Extern<Vector2F>::Save(OArchive& _archive, const Vector2F* _values, size_t _count) {
    _archive << MakeArray(&_values->x, 2 * _count);
}
void Extern<Vector2F>::Load(IArchive& _archive, Vector2F* _values, size_t _count, uint32_t _version) {
    (void)_version;
    _archive >> MakeArray(&_values->x, 2 * _count);
}

void Extern<Vector3F>::Save(OArchive& _archive, const Vector3F* _values, size_t _count) {
    _archive << MakeArray(&_values->x, 3 * _count);
}
void Extern<Vector3F>::Load(IArchive& _archive, Vector3F* _values, size_t _count, uint32_t _version) {
    (void)_version;
    _archive >> MakeArray(&_values->x, 3 * _count);
}

void Extern<Vector4F>::Save(OArchive& _archive, const Vector4F* _values, size_t _count) {
    _archive << MakeArray(&_values->x, 4 * _count);
}
void Extern<Vector4F>::Load(IArchive& _archive, Vector4F* _values, size_t _count, uint32_t _version) {
    (void)_version;
    _archive >> MakeArray(&_values->x, 4 * _count);
}

void Extern<QuaternionF>::Save(OArchive& _archive, const QuaternionF* _values, size_t _count) {
    _archive << MakeArray(&_values->x, 4 * _count);
}
void Extern<QuaternionF>::Load(IArchive& _archive, QuaternionF* _values, size_t _count, uint32_t _version) {
    (void)_version;
    _archive >> MakeArray(&_values->x, 4 * _count);
}

void Extern<ScalableTransform>::Save(OArchive& _archive, const ScalableTransform* _values, size_t _count) {
    _archive << MakeArray(&_values->translation.x, 10 * _count);
}
void Extern<ScalableTransform>::Load(IArchive& _archive, ScalableTransform* _values, size_t _count, uint32_t _version) {
    (void)_version;
    _archive >> MakeArray(&_values->translation.x, 10 * _count);
}

void Extern<BoundingBox3F>::Save(OArchive& _archive, const BoundingBox3F* _values, size_t _count) {
    _archive << MakeArray(&_values->lower_corner.x, 6 * _count);
}
void Extern<BoundingBox3F>::Load(IArchive& _archive, BoundingBox3F* _values, size_t _count, uint32_t _version) {
    (void)_version;
    _archive >> MakeArray(&_values->lower_corner.x, 6 * _count);
}

void Extern<RectFloat>::Save(OArchive& _archive, const RectFloat* _values, size_t _count) {
    _archive << MakeArray(&_values->left, 4 * _count);
}
void Extern<RectFloat>::Load(IArchive& _archive, RectFloat* _values, size_t _count, uint32_t _version) {
    (void)_version;
    _archive >> MakeArray(&_values->left, 4 * _count);
}

void Extern<RectInt>::Save(OArchive& _archive, const RectInt* _values, size_t _count) {
    _archive << MakeArray(&_values->left, 4 * _count);
}
void Extern<RectInt>::Load(IArchive& _archive, RectInt* _values, size_t _count, uint32_t _version) {
    (void)_version;
    _archive >> MakeArray(&_values->left, 4 * _count);
}
}  // namespace vox
