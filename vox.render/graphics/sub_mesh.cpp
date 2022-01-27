//
//  sub_mesh.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/27.
//

#include "sub_mesh.h"

namespace vox {
SubMesh::SubMesh(uint32_t start, uint32_t count,
                 wgpu::PrimitiveTopology topology):
_start(start),
_count(count),
_topology(topology) {
}

uint32_t SubMesh::start() {
    return _start;
}

uint32_t SubMesh::count() {
    return _count;
}

wgpu::PrimitiveTopology SubMesh::topology() {
    return _topology;
}

}
