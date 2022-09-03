//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/particle/wgsl/wgsl_particle_common.h"
#include "vox.render/shaderlib/wgsl_cache.h"

namespace vox {
class WGSLParticleEmission : public WGSLCache {
public:
    WGSLParticleEmission();

private:
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) override;

    WGSLParticleCommon _particleCommon;
};

}  // namespace vox