//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/particle/wgsl/wgsl_particle_common.h"
#include "vox.render/particle/wgsl/wgsl_particle_noise.h"
#include "vox.render/particle/wgsl/wgsl_particle_sdf.h"
#include "vox.render/shaderlib/wgsl_cache.h"

namespace vox {
class WGSLParticleSimulation : public WGSLCache {
public:
    WGSLParticleSimulation();

private:
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) override;

    WGSLParticleCommon _particleCommon;
    WGSLParticleNoise _particleNoise;
    WGSLParticleSDF _particleSDF;
};

}  // namespace vox