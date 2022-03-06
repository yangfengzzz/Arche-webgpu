//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_particle_emission.h"
#include "particle/particle_manager.h"
#include "filesystem.h"
#include <fmt/core.h>

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
        encoder.addUniformBinding("u_randomBuffer", fmt::format("array<f32, {}>", particleCount * 3));
        
        encoder.addFunction(fmt::format("fn pushParticle(position: vec3<f32>, velocity: vec3<f32>, age: f32,"
                            " particles: ptr<storage, array<TParticle, {} >, read_write>,"
                            " write_count: ptr<storage, Counter, read_write>) {{\n"
                            "    // Emit particle id.\n"
                            "    let id = atomicAdd(&(*write_count).counter, 1u);\n"
                            "    \n"
                            "    var p = TParticle();\n"
                            "    p.position = vec4<f32>(position, 1.0);\n"
                            "    p.velocity = vec4<f32>(velocity, 0.0);\n"
                            "    p.start_age = age;\n"
                            "    p.age = age;\n"
                            "    p.id = id;\n"
                            "    \n"
                            "    (*particles)[id] = p;\n"
                            "}}", particleCount));
        
        encoder.addFunction(fmt::format("fn createParticle(uEmitCount: u32, uEmitterType: u32, uEmitterPosition: vec3<f32>,"
                            " uEmitterDirection: vec3<f32>, uEmitterRadius: f32, uParticleMinAge: f32, uParticleMaxAge: f32,"
                            " write_count: ptr<storage, Counter, read_write>,"
                            " particles: ptr<storage, array<TParticle, {} >, read_write>, "
                            " randbuffer: array<f32, {} >, gid: u32) {{\n"
                            "    // Random vector.\n"
                            "    let rid = 3u * gid;\n"
                            "    let rn = vec3<f32>(randbuffer[rid], randbuffer[rid+1u], randbuffer[rid+2u]);\n"
                            "    \n"
                            "    // Position\n"
                            "    var pos = uEmitterPosition;\n"
                            "    if (uEmitterType == 1u) {{\n"
                            "        //pos += disk_distribution(uEmitterRadius, rn.xy);\n"
                            "        pos = pos + disk_even_distribution(uEmitterRadius, gid, uEmitCount);\n"
                            "    }} else if (uEmitterType == 2) {{\n"
                            "        pos = pos + sphere_distribution(uEmitterRadius, rn.xy);\n"
                            "    }} else if (uEmitterType == 3) {{\n"
                            "        pos = pos + ball_distribution(uEmitterRadius, rn);\n"
                            "    }}\n"
                            "    \n"
                            "    // Velocity\n"
                            "    var vel = uEmitterDirection;\n"
                            "    \n"
                            "    // Age\n"
                            "    // The age is set by thread groups to assure we have a number of particles\n"
                            "    // factors of groupWidth, this method is safe but prevents continuous emission.\n"
                            "    // const float group_rand = randbuffer[gid];\n"
                            "    // [As the threadgroup are not full, some dead particles might appears if not\n"
                            "    // skipped in following stages].\n"
                            "    let single_rand = randbuffer[gid];\n"
                            "    \n"
                            "    let age = mix( uParticleMinAge, uParticleMaxAge, single_rand);\n"
                            "    \n"
                            "    pushParticle(pos, vel, age, particles, write_count);\n"
                            "}}\n", particleCount, particleCount * 3));
                
        encoder.addEntry({ParticleManager::PARTICLES_KERNEL_GROUP_WIDTH, 1, 1}, [&](std::string &source){
            source += "if (global_id.x < u_emitterData.emitCount) {\n";
            source += "    createParticle(u_emitterData.emitCount,\n";
            source += "     u_emitterData.emitterType,\n";
            source += "     u_emitterData.emitterPosition,\n";
            source += "     u_emitterData.emitterDirection,\n";
            source += "     u_emitterData.emitterRadius,\n";
            source += "     u_emitterData.particleMinAge,\n";
            source += "     u_emitterData.particleMaxAge,\n";
            source += "     &u_readAtomicBuffer,\n";
            source += "     &u_readConsumeBuffer,\n";
            source += "     u_randomBuffer,\n";
            source += "     global_id.x)\n";
            source += "}\n";
        }, {{"global_id", BuiltInType::GlobalInvocationID}});
        encoder.flush();
    }
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

}
