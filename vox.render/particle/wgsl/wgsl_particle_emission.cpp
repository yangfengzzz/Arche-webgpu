//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_particle_emission.h"
#include "particle/particle_manager.h"
#include "filesystem.h"

namespace vox {
void WGSLParticleEmission::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source.clear();
    _bindGroupInfo.clear();
    {
        auto encoder = createSourceEncoder(wgpu::ShaderStage::Compute);
        _particleCommon(encoder, macros);
        
        encoder.addStruct("struct ParticleEmitterData {\n"
                          "    emitterPosition: vec3<f32>;\n"
                          "    emitCount: u32;\n"
                          "    emitterDirection: vec3<f32>;\n"
                          "    emitterType: u32;\n"
                          "    emitterRadius: f32;\n"
                          "    particleMinAge: f32;\n"
                          "    particleMaxAge: f32;\n"
                          "    pad: f32;\n"
                          "}\n");
        encoder.addUniformBinding("u_emitterData", "ParticleEmitterData");
        
        encoder.addStruct("struct Counter {\n"
                          "counter: atomic<u32>;\n"
                          "}\n");
        encoder.addStorageBufferBinding("u_readAtomicBuffer", "Counter", false);
        
        encoder.addStorageBufferBinding("u_readConsumeBuffer", "array<TParticle, {}>", false);
        encoder.addUniformBinding("u_randomBuffer", "array<f32, {}>");
        
        encoder.addFunction(fs::readShader("particle/particle_emission.wgsl"));
        
        encoder.addEntry({ParticleManager::PARTICLES_KERNEL_GROUP_WIDTH, 1, 1}, [&](std::string &source){
            source += "if (global_id < u_emitterData.emitCount) {\n";
            source += "    createParticle(uEmitCount,\n";
            source += "     u_emitterData.emitterType,\n";
            source += "     u_emitterData.emitterPosition,\n";
            source += "     u_emitterData.emitterDirection,\n";
            source += "     u_emitterData.emitterRadius,\n";
            source += "     u_emitterData.particleMinAge,\n";
            source += "     u_emitterData.particleMaxAge,\n";
            source += "     u_readAtomicBuffer.counter,\n";
            source += "     u_readConsumeBuffer,\n";
            source += "     u_randomBuffer,\n";
            source += "     global_id)\n";
            source += "}\n";
        }, {{"global_id", BuiltInType::GlobalInvocationID}});
        encoder.flush();
    }
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

}
