//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_particle_common.h"

#include "filesystem.h"

namespace vox {
void WGSLParticleCommon::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros) {
    std::string particleStruct = fs::readShader("particle/particle_struct.wgsl");
    encoder.addStruct(particleStruct);

    std::string particleMath = fs::readShader("particle/particle_math.wgsl");
    encoder.addFunction(particleMath);
}

}  // namespace vox
