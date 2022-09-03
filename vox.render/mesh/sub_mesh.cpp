//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sub_mesh.h"

namespace vox {
SubMesh::SubMesh(uint32_t start, uint32_t count, wgpu::PrimitiveTopology topology)
    : _start(start), _count(count), _topology(topology) {}

uint32_t SubMesh::start() const { return _start; }

uint32_t SubMesh::count() const { return _count; }

wgpu::PrimitiveTopology SubMesh::topology() const { return _topology; }

}  // namespace vox
