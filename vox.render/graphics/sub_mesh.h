//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef sub_mesh_hpp
#define sub_mesh_hpp

#include <dawn/webgpu_cpp.h>

namespace vox {
/**
 * Sub-mesh, mainly contains drawing information.
 */
class SubMesh {
public:
    /**
     * Create a sub-mesh.
     * @param start - Start drawing offset
     * @param count - Drawing count
     * @param topology - Drawing topology
     */
    SubMesh(uint32_t start = 0, uint32_t count = 0,
            wgpu::PrimitiveTopology topology = wgpu::PrimitiveTopology::TriangleList);
    
    uint32_t start();
    
    uint32_t count();
    
    wgpu::PrimitiveTopology topology();
    
private:
    /** Start drawing offset. */
    uint32_t _start;
    /** Drawing count. */
    uint32_t _count;
    /** Drawing topology. */
    wgpu::PrimitiveTopology _topology;
};

}

#endif /* sub_mesh_hpp */
