//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_particle_noise.h"
#include "filesystem.h"

namespace vox {
void WGSLParticleNoise::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros) {
    std::string particlePerlin = fs::readShader("particle/particle_perlin.wgsl");
    encoder.addFunction(particlePerlin);
    
    std::string particleCurlNoise = fs::readShader("particle/particle_curl_noise.wgsl");
    encoder.addFunction(particleCurlNoise);
}

}
