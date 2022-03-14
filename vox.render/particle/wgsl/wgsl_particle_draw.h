//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef wgsl_particle_draw_hpp
#define wgsl_particle_draw_hpp

#include "wgsl_cache.h"
#include "shaderlib/functors/wgsl_common_frag.h"

namespace vox {
//MARK: - Unlit Vertex Code
class WGSLParticleVertex : public WGSLCache {
public:
    WGSLParticleVertex();
        
private:
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) override;
    
    WGSLCommonFrag _commonFrag;
};

//MARK: - Unlit Fragment Code
class WGSLParticleFragment : public WGSLCache {
public:
    WGSLParticleFragment();
        
private:
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) override;
};

}
#endif /* wgsl_particle_draw_hpp */
