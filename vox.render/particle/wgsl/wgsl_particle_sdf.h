//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef wgsl_particle_sdf_hpp
#define wgsl_particle_sdf_hpp

#include "shaderlib/wgsl_encoder.h"

namespace vox {
class WGSLParticleSDF {
public:
    void operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros);
};

}
#endif /* wgsl_particle_sdf_hpp */
