//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/particle/wgsl/wgsl_particle_noise.h"

#include "vox.render/platform/filesystem.h"

namespace vox {
void WGSLParticleNoise::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros) {
    std::string particlePerlin = fs::ReadShader("particle/particle_perlin.wgsl");
    encoder.addFunction(particlePerlin);

    std::string particleCurlNoise = fs::ReadShader("particle/particle_curl_noise.wgsl");
    encoder.addFunction(particleCurlNoise);
}

}  // namespace vox
