//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_particle_simulation.h"
#include "particle/particle_manager.h"
#include "filesystem.h"
#include <fmt/core.h>

namespace vox {
WGSLParticleSimulation::WGSLParticleSimulation() {
}

void WGSLParticleSimulation::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source.clear();
    _bindGroupInfo.clear();
    {
        auto encoder = createSourceEncoder(wgpu::ShaderStage::Compute);
        _particleCommon(encoder, macros);
        encoder.addStruct("struct ParticleSimulationData {\n"
                          "    timeStep:f32;\n"
                          "    boundingVolumeType:i32;\n"
                          "    bboxSize:f32;\n"
                          "    scatteringFactor:f32;\n"
                          "    vectorFieldFactor:f32;\n"
                          "    curlNoiseFactor:f32;\n"
                          "    curlNoiseScale:f32;\n"
                          "    velocityFactor:f32;\n"
                          "};\n");
        encoder.addUniformBinding("u_simulationData", "ParticleSimulationData");
        
        encoder.addStruct("struct Counter {\n"
                          "counter: atomic<u32>;\n"
                          "};\n");
        encoder.addStorageBufferBinding("u_readAtomicBuffer", "Counter", false);
        encoder.addStorageBufferBinding("u_writeAtomicBuffer", "Counter", false);
        
        auto particleCount = *macros.macroConstant(PARTICLE_COUNT);
        encoder.addStorageBufferBinding("u_readConsumeBuffer", fmt::format("array<TParticle, {}>", particleCount), false);
        encoder.addStorageBufferBinding("u_writeConsumeBuffer", fmt::format("array<TParticle, {}>", particleCount), false);
        encoder.addUniformBinding("u_randomBuffer", fmt::format("array<vec4<f32>, {}>", particleCount/2));
        
        encoder.addFunction(fmt::format("fn popParticle(read_count: ptr<storage, atomic<u32> >, read_particles: ptr<storage, array<TParticle, {}> >, "
                                        "index: u32) -> TParticle {{\n"
                                        "    atomicSub(read_count, 1u);\n"
                                        "    return (*read_particles)[index];\n"
                                        "}}\n", particleCount));
        encoder.addFunction(fmt::format("fn pushParticle(p: TParticle, write_count: ptr<storage, atomic<u32> >, "
                                        "write_particles: ptr<storage, array<TParticle, {}> >) {{\n"
                                        "    let index = atomicAdd(write_count, 1u);\n"
                                        "    (*write_particles)[index] = p;\n"
                                        "}}\n", particleCount));
        
        encoder.addFunction("fn updatedAge(p: TParticle, uTimeStep: f32) -> f32 {\n"
                            "    return clamp(p.age - uTimeStep, 0.0, p.start_age);\n"
                            "}\n");
                            
        encoder.addFunction("fn updateParticle(p: ptr<function, TParticle>, pos: vec3<f32>, vel: vec3<f32>, age: f32) {\n"
                            "    p.position.xyz = pos;\n"
                            "    p.velocity.xyz = vel;\n"
                            "    p.age = age;\n"
                            "}\n");
                            
        encoder.addFunction("fn calculateScattering(uScatteringFactor:f32,\n"
                            "                        randbuffer: array<f32, 10>,\n"
                            "                            gid:uint)->vec3<f32> {\n"
                            "    var randforce = vec3<f32>(randbuffer[gid], randbuffer[gid+1u], randbuffer[gid+2u]);\n"
                            "    randforce = 2.0 * randforce - 1.0;\n"
                            "    return uScatteringFactor * randforce;\n"
                            "}\n");
                            
        encoder.addFunction("fn calculateRepulsion(p:TParticle)->vec3<f32> {\n"
                            "    let push = vec3<f32>(0.0);    \n"
                            "    return push;\n"
                            "}\n");
                            
        encoder.addFunction("fn calculateTargetMesh(p:TParticle)->vec3<f32> {\n"
                            "    let pull = vec3<f32>(0.0);\n"
                            "    return pull;\n"
                            "}\n");
                            
        encoder.addFunction("fn calculateVectorField(p:TParticle,\n"
                            "                             uVectorFieldFactor:f32,\n"
                            "                            uVectorFieldTexture:texture_3d<f32>,\n"
                            "                             uVectorFieldSampler:sampler) {\n"
                            "    let extent = 0.5 * textureDimensions(uVectorFieldTexture);\n"
                            "    let texcoord = (p.position.xyz + extent) / (2.0 * extent);\n"
                            "    let vfield = textureSample(uVectorFieldTexture, uVectorFieldSampler, texcoord).xyz;\n"
                            "    \n"
                            "    return uVectorFieldFactor * vfield;\n"
                            "}\n");
                            
        encoder.addFunction("fn calculateCurlNoise(p:TParticle,\n"
                            "                      uCurlNoiseScale:f32,\n"
                            "                      uCurlNoiseFactor:f32,\n"
                            "                      uPerlinNoisePermutationSeed:i32)->vec3<f32> {\n"
                            "    let curl_velocity = compute_curl(p.position.xyz * uCurlNoiseScale, uPerlinNoisePermutationSeed);\n"
                            "    return uCurlNoiseFactor * curl_velocity;\n"
                            "}\n");
                            
        encoder.addFunction("fn collideSphere(r:f32, center:vec3<f32>, pos: ptr<function, vec3<f32> >, vel: ptr<function, vec3<f32> >) {\n"
                            "    let p = *pos - center;\n"
                            "    \n"
                            "    let dp = dot(p, p);\n"
                            "    let r2 = r*r;\n"
                            "    \n"
                            "    if (dp > r2) {\n"
                            "        let n = -p * inverseSqrt(dp);\n"
                            "        *vel = reflect(*vel, n);\n"
                            "        \n"
                            "        *pos = center - r*n;\n"
                            "    }\n"
                            "}\n");
                            
        encoder.addFunction("fn collideBox(corner:vec3<f32>, center:vec3<f32>, pos: ptr<function, vec3<f32> >, vel: ptr<function, vec3<f32> >) {\n"
                            "    let p = *pos - center;\n"
                            "    \n"
                            "    if (p.x < -corner.x) {\n"
                            "        p.x = -corner.x;\n"
                            "        *vel = reflect(*vel, vec3<f32>(1.0, 0.0, 0.0));\n"
                            "    }\n"
                            "    \n"
                            "    if (p.x > corner.x) {\n"
                            "        p.x = corner.x;\n"
                            "        *vel = reflect(*vel, vec3<f32>(-1.0, 0.0, 0.0));\n"
                            "    }\n"
                            "    \n"
                            "    if (p.y < -corner.y) {\n"
                            "        p.y = -corner.y;\n"
                            "        *vel = reflect(*vel, vec3<f32>(0.0, 1.0, 0.0));\n"
                            "    }\n"
                            "    \n"
                            "    if (p.y > corner.y) {\n"
                            "        p.y = corner.y;\n"
                            "        *vel = reflect(*vel, vec3<f32>(0.0, -1.0, 0.0));\n"
                            "    }\n"
                            "    \n"
                            "    if (p.z < -corner.z) {\n"
                            "        p.z = -corner.z;\n"
                            "        *vel = reflect(*vel, vec3<f32>(0.0, 0.0, 1.0));\n"
                            "    }\n"
                            "    \n"
                            "    if (p.z > corner.z) {\n"
                            "        p.z = corner.z;\n"
                            "        *vel = reflect(*vel, vec3<f32>(0.0, 0.0, -1.0));\n"
                            "    }\n"
                            "    \n"
                            "    *pos = p + center;\n"
                            "}\n");
                            
        encoder.addFunction("fn collisionHandling(pos: ptr<function, vec3<f32> >, vel: ptr<function, vec3<f32> >\n"
                            "                    uBoundingVolume:i32,\n"
                            "                        uBBoxSize:f32) {\n"
                            "    let r = 0.5 * uBBoxSize;\n"
                            "    \n"
                            "    if (uBoundingVolume == 0)\n"
                            "        collideSphere(r, vec3<f32>(0.0), pos, vel);\n"
                            "    else\n"
                            "        if (uBoundingVolume == 1)\n"
                            "            collideBox(vec3<f32>(r), vec3<f32>(0.0), pos, vel);\n"
                            "}\n");
        
        encoder.addEntry({ParticleManager::PARTICLES_KERNEL_GROUP_WIDTH, 1, 1}, [&](std::string &source){
            source += "    // Local copy of the particle.\n"
            "    let p = popParticle(uReadCounter,\n"
            "                              uReadParticle,\n"
            "                              gid);\n"
            "    \n"
            "    let age = updatedAge(p, uTimeStep);\n"
            "    \n"
            "    if (age > 0.0) {\n"
            "        // Calculate external forces.\n"
            "        var force = vec3<f32>(0.0);\n"
            "        \n"
            "        if (needParticleScattering) {\n"
            "            force += calculateScattering(uScatteringFactor, uRandomBuffer, gid);\n"
            "        }\n"
            "        force += calculateRepulsion(p);\n"
            "        force += calculateTargetMesh(p);\n"
            "        if (needParticleVectorField) {\n"
            "            force += calculateVectorField(p, uVectorFieldFactor,\n"
            "                                          uVectorFieldTexture, uVectorFieldSampler);\n"
            "        }\n"
            "        if (needParticleCurlNoise) {\n"
            "            force += calculateCurlNoise(p, uCurlNoiseScale,\n"
            "                                        uCurlNoiseFactor, uPerlinNoisePermutationSeed);\n"
            "        }\n"
            "        \n"
            "        // Integrations vectors.\n"
            "        let dt = vec3<f32>(uTimeStep);\n"
            "        var velocity = p.velocity.xyz;\n"
            "        var position = p.position.xyz;\n"
            "        \n"
            "        // Integrate velocity.\n"
            "        velocity = fma(force, dt, velocity);\n"
            "        \n"
            "        if (needParticleVelocityControl) {\n"
            "            velocity = uVelocityFactor * normalize(velocity);\n"
            "        }\n"
            "        \n"
            "        // Integrate position.\n"
            "        position = fma(velocity, dt, position);\n"
            "        \n"
            "        // Handle collisions.\n"
            "        collisionHandling(position, velocity,\n"
            "                          uBoundingVolume,\n"
            "                          uBBoxSize);\n"
            "        \n"
            "        // Update the particle.\n"
            "        updateParticle(p, position, velocity, age);\n"
            "        \n"
            "        // Save it in buffer.\n"
            "        pushParticle(p, uWriteCounter, uWriteParticle);\n"
            "    }\n";
        }, {{"global_id", BuiltInType::GlobalInvocationID}});
        encoder.flush();
    }
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

}
