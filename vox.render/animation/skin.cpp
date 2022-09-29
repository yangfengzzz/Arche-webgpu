//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/animation/skin.h"

#include "vox.base/containers/vector_archive.h"
#include "vox.simd_math/simd_math_archive.h"

namespace vox::io {

void Extern<Skin::Part>::Save(OArchive& _archive, const Skin::Part* _parts, size_t _count) {
    for (size_t i = 0; i < _count; ++i) {
        const Skin::Part& part = _parts[i];
        _archive << part.positions;
        _archive << part.normals;
        _archive << part.tangents;
        _archive << part.uvs;
        _archive << part.colors;
        _archive << part.joint_indices;
        _archive << part.joint_weights;
    }
}

void Extern<Skin::Part>::Load(IArchive& _archive, Skin::Part* _parts, size_t _count, uint32_t _version) {
    (void)_version;
    for (size_t i = 0; i < _count; ++i) {
        Skin::Part& part = _parts[i];
        _archive >> part.positions;
        _archive >> part.normals;
        _archive >> part.tangents;
        _archive >> part.uvs;
        _archive >> part.colors;
        _archive >> part.joint_indices;
        _archive >> part.joint_weights;
    }
}

void Extern<Skin>::Save(OArchive& _archive, const Skin* _meshes, size_t _count) {
    for (size_t i = 0; i < _count; ++i) {
        const Skin& mesh = _meshes[i];
        _archive << mesh.parts;
        _archive << mesh.triangle_indices;
        _archive << mesh.joint_remaps;
        _archive << mesh.inverse_bind_poses;
    }
}

void Extern<Skin>::Load(IArchive& _archive, Skin* _meshes, size_t _count, uint32_t _version) {
    (void)_version;
    for (size_t i = 0; i < _count; ++i) {
        Skin& mesh = _meshes[i];
        _archive >> mesh.parts;
        _archive >> mesh.triangle_indices;
        _archive >> mesh.joint_remaps;
        _archive >> mesh.inverse_bind_poses;
    }
}
}  // namespace vox::io
