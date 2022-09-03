//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_particle_emission.h"
#include "particle/particle_manager.h"
#include "filesystem.h"
#include <spdlog/fmt/fmt.h>

namespace vox {
WGSLParticleEmission::WGSLParticleEmission() {
}

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
                          "};\n");
        encoder.addUniformBinding("u_emitterData", "ParticleEmitterData");
        
        encoder.addStruct("struct Counter {\n"
                          "counter: atomic<u32>;\n"
                          "};\n");
        encoder.addStorageBufferBinding("u_readAtomicBuffer", "Counter", false);
        
        auto particleCount = *macros.macroConstant(PARTICLE_COUNT);
        encoder.addStorageBufferBinding("u_readConsumeBuffer", fmt::format("array<TParticle, {}>", particleCount), false);
        encoder.addUniformBinding("u_randomBuffer", "array<vec4<f32>, 256>");
                
        encoder.addEntry({ParticleManager::PARTICLES_KERNEL_GROUP_WIDTH, 1, 1}, [&](std::string &source){
            // source += "atomicStore(&u_readAtomicBuffer.counter, 0u);\n";
            // source += "storageBarrier();\n";
            source += "if (global_id.x < u_emitterData.emitCount) {\n";
            source += " // Random vector.\n"
            "    let rn = vec3<f32>(u_randomBuffer[global_id.x / 256u].x, u_randomBuffer[global_id.x / 256u].y, u_randomBuffer[global_id.x / 256u].z);\n"
            "    \n"
            "    // Position\n"
            "    var pos = u_emitterData.emitterPosition;\n"
            "    if (u_emitterData.emitterType == 1u) {\n"
            "        //pos += disk_distribution(uEmitterRadius, rn.xy);\n"
            "        pos = pos + disk_even_distribution(u_emitterData.emitterRadius, global_id.x, u_emitterData.emitCount);\n"
            "    } else if (u_emitterData.emitterType == 2u) {\n"
            "        pos = pos + sphere_distribution(u_emitterData.emitterRadius, rn.xy);\n"
            "    } else if (u_emitterData.emitterType == 3u) {\n"
            "        pos = pos + ball_distribution(u_emitterData.emitterRadius, rn);\n"
            "    }\n"
            "    \n"
            "    // Velocity\n"
            "    var vel = u_emitterData.emitterDirection;\n"
            "    \n"
            "    // Age\n"
            "    // The age is set by thread groups to assure we have a number of particles\n"
            "    // factors of groupWidth, this method is safe but prevents continuous emission.\n"
            "    // const float group_rand = randbuffer[gid];\n"
            "    // [As the threadgroup are not full, some dead particles might appears if not\n"
            "    // skipped in following stages].\n"
            "    \n"
            "    let age = mix( u_emitterData.particleMinAge, u_emitterData.particleMaxAge, u_randomBuffer[global_id.x / 256u].w);\n"
            "\n"
            "    // Emit particle id.\n"
            "    let id = atomicAdd(&u_readAtomicBuffer.counter, 1u);\n"
            "    \n"
            "    var p = TParticle();\n"
            "    p.position = vec4<f32>(pos, 1.0);\n"
            "    p.velocity = vec4<f32>(vel, 0.0);\n"
            "    p.start_age = age;\n"
            "    p.age = age;\n"
            "    p.id = id;\n"
            "    \n"
            "    u_readConsumeBuffer[id] = p;\n";
            source += "}\n";
        }, {{"global_id", BuiltInType::GlobalInvocationID}});
        encoder.flush();
    }
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

}
