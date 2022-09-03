//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/particle/wgsl/wgsl_particle_sdf.h"

#include "vox.render/platform/filesystem.h"

namespace vox {
void WGSLParticleSDF::operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros) {
    std::string particleDistanceUtils = fs::ReadShader("particle/particle_distance_utils.wgsl");
    encoder.addFunction(particleDistanceUtils);

    std::string particleDistanceFunc = fs::ReadShader("particle/particle_distance_func.wgsl");
    encoder.addFunction(particleDistanceFunc);
}

}  // namespace vox
