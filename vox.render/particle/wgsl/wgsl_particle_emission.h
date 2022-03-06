//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef wgsl_particle_emission_hpp
#define wgsl_particle_emission_hpp

#include "shaderlib/wgsl_cache.h"
#include "wgsl_particle_common.h"

namespace vox {
class WGSLParticleEmission : public WGSLCache {
public:
    WGSLParticleEmission();
    
private:
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) override;
    
    WGSLParticleCommon _particleCommon;
};

}
#endif /* wgsl_particle_emission_hpp */
