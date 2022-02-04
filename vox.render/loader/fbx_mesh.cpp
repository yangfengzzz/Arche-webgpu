//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "fbx_mesh.h"
#include <ozz/base/containers/vector_archive.h>
#include <ozz/base/memory/allocator.h>

#include <ozz/base/io/archive.h>

#include <ozz/base/maths/math_archive.h>
#include <ozz/base/maths/simd_math_archive.h>

using namespace ozz;
using namespace io;

namespace ozz {
namespace io {

void Extern<loader::Mesh::Part>::Save(OArchive& _archive,
                                      const loader::Mesh::Part* _parts,
                                      size_t _count) {
    for (size_t i = 0; i < _count; ++i) {
        const loader::Mesh::Part& part = _parts[i];
        _archive << part.positions;
        _archive << part.normals;
        _archive << part.tangents;
        _archive << part.uvs;
        _archive << part.colors;
        _archive << part.joint_indices;
        _archive << part.joint_weights;
    }
}

void Extern<loader::Mesh::Part>::Load(IArchive& _archive,
                                      loader::Mesh::Part* _parts, size_t _count,
                                      uint32_t _version) {
    (void)_version;
    for (size_t i = 0; i < _count; ++i) {
        loader::Mesh::Part& part = _parts[i];
        _archive >> part.positions;
        _archive >> part.normals;
        _archive >> part.tangents;
        _archive >> part.uvs;
        _archive >> part.colors;
        _archive >> part.joint_indices;
        _archive >> part.joint_weights;
    }
}

void Extern<loader::Mesh>::Save(OArchive& _archive, const loader::Mesh* _meshes,
                                size_t _count) {
    for (size_t i = 0; i < _count; ++i) {
        const loader::Mesh& mesh = _meshes[i];
        _archive << mesh.parts;
        _archive << mesh.triangle_indices;
        _archive << mesh.joint_remaps;
        _archive << mesh.inverse_bind_poses;
    }
}

void Extern<loader::Mesh>::Load(IArchive& _archive, loader::Mesh* _meshes,
                                size_t _count, uint32_t _version) {
    (void)_version;
    for (size_t i = 0; i < _count; ++i) {
        loader::Mesh& mesh = _meshes[i];
        _archive >> mesh.parts;
        _archive >> mesh.triangle_indices;
        _archive >> mesh.joint_remaps;
        _archive >> mesh.inverse_bind_poses;
    }
}
}  // namespace io
} // ozz
