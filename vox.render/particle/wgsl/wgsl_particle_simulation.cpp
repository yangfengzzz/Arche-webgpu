//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_particle_simulation.h"

#include <spdlog/fmt/fmt.h>

#include "filesystem.h"
#include "particle/particle_manager.h"

namespace vox {
WGSLParticleSimulation::WGSLParticleSimulation() {}

void WGSLParticleSimulation::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source.clear();
    _bindGroupInfo.clear();
    {
        auto encoder = createSourceEncoder(wgpu::ShaderStage::Compute);
        _particleCommon(encoder, macros);
        _particleNoise(encoder, macros);
        _particleSDF(encoder, macros);

        encoder.addStruct(
                "struct ParticleSimulationData {\n"
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

        encoder.addStruct(
                "struct Counter {\n"
                "counter: atomic<u32>;\n"
                "};\n");
        encoder.addStorageBufferBinding("u_readAtomicBuffer", "Counter", false);
        encoder.addStorageBufferBinding("u_writeAtomicBuffer", "Counter", false);

        auto particleCount = *macros.macroConstant(PARTICLE_COUNT);
        encoder.addStorageBufferBinding("u_readConsumeBuffer", fmt::format("array<TParticle, {}>", particleCount),
                                        false);
        encoder.addStorageBufferBinding("u_writeConsumeBuffer", fmt::format("array<TParticle, {}>", particleCount),
                                        false);
        encoder.addUniformBinding("u_randomBuffer", "array<vec4<f32>, 256>");

        encoder.addFunction(
                "fn popParticle(index: u32) -> TParticle {\n"
                "    atomicSub(&u_readAtomicBuffer.counter, 1u);\n"
                "    return u_readConsumeBuffer[index];\n"
                "}\n");
        encoder.addFunction(
                "fn pushParticle(p: TParticle) {\n"
                "    let index = atomicAdd(&u_writeAtomicBuffer.counter, 1u);\n"
                "    u_writeConsumeBuffer[index] = p;\n"
                "}\n");

        encoder.addFunction(
                "fn updatedAge(p: TParticle, uTimeStep: f32) -> f32 {\n"
                "    return clamp(p.age - uTimeStep, 0.0, p.start_age);\n"
                "}\n");

        encoder.addFunction(
                "fn updateParticle(p: ptr<function, TParticle>, pos: vec3<f32>, vel: vec3<f32>, age: f32) {\n"
                "    (*p).position = vec4<f32>(pos, (*p).position.w);\n"
                "    (*p).velocity = vec4<f32>(vel, (*p).velocity.w);\n"
                "    (*p).age = age;\n"
                "}\n");

        encoder.addFunction(
                "fn calculateScattering(global_id:u32)->vec3<f32> {\n"
                "    var randforce = vec3<f32>(u_randomBuffer[global_id / 256u].x, u_randomBuffer[global_id / 256u].y, "
                "u_randomBuffer[global_id / 256u].z);\n"
                "    randforce = 2.0 * randforce - 1.0;\n"
                "    return u_simulationData.scatteringFactor * randforce;\n"
                "}\n");

        encoder.addFunction(
                "fn calculateRepulsion(p:TParticle)->vec3<f32> {\n"
                "    let push = vec3<f32>(0.0);    \n"
                "    return push;\n"
                "}\n");

        encoder.addFunction(
                "fn calculateTargetMesh(p:TParticle)->vec3<f32> {\n"
                "    let pull = vec3<f32>(0.0);\n"
                "    return pull;\n"
                "}\n");

        encoder.addFunction(
                "fn calculateVectorField(p:TParticle,\n"
                "                        uVectorFieldFactor:f32,\n"
                "                        uVectorFieldTexture:texture_3d<f32>,\n"
                "                        uVectorFieldSampler:sampler)->vec3<f32> {\n"
                "    let dim = textureDimensions(uVectorFieldTexture);"
                "    let extent = vec3<f32>(0.5 * f32(dim.x), 0.5 * f32(dim.y), 0.5 * f32(dim.z));\n"
                "    let texcoord = (p.position.xyz + extent) / (2.0 * extent);\n"
                "    let vfield = textureSample(uVectorFieldTexture, uVectorFieldSampler, texcoord).xyz;\n"
                "    \n"
                "    return uVectorFieldFactor * vfield;\n"
                "}\n");

        encoder.addFunction(
                "fn calculateCurlNoise(p:TParticle)->vec3<f32> {\n"
                "    let curl_velocity = compute_curl(p.position.xyz * u_simulationData.curlNoiseScale, 0.0);\n"
                "    return u_simulationData.curlNoiseFactor * curl_velocity;\n"
                "}\n");

        encoder.addFunction(
                "fn collideSphere(r:f32, center:vec3<f32>, pos: ptr<function, vec3<f32> >, vel: ptr<function, "
                "vec3<f32> >) {\n"
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

        encoder.addFunction(
                "fn collideBox(corner:vec3<f32>, center:vec3<f32>, pos: ptr<function, vec3<f32> >, vel: ptr<function, "
                "vec3<f32> >) {\n"
                "    var p = *pos - center;\n"
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

        encoder.addFunction(
                "fn collisionHandling(pos: ptr<function, vec3<f32> >, vel: ptr<function, vec3<f32> >) {\n"
                "    let r = 0.5 * u_simulationData.bboxSize;\n"
                "    \n"
                "    if (u_simulationData.boundingVolumeType == 0) {\n"
                "        collideSphere(r, vec3<f32>(0.0), pos, vel);\n"
                "    } else {\n"
                "        if (u_simulationData.boundingVolumeType == 1) {\n"
                "            collideBox(vec3<f32>(r), vec3<f32>(0.0), pos, vel);\n"
                "        }\n"
                "    }\n"
                "}\n");

        encoder.addEntry({ParticleManager::PARTICLES_KERNEL_GROUP_WIDTH, 1, 1},
                         [&](std::string& source) {
                             // Local copy of the particle.
                             source +=
                                     "    var p = popParticle(global_id.x);\n"
                                     "    \n"
                                     "    let age = updatedAge(p, u_simulationData.timeStep);\n"
                                     "    \n"
                                     "    if (age > 0.0) {\n"
                                     "        // Calculate external forces.\n"
                                     "        var force = vec3<f32>(0.0);\n"
                                     "        \n";
                             if (macros.contains(NEED_PARTICLE_SCATTERING)) {
                                 source += "force = force + calculateScattering(global_id.x);\n";
                             };

                             source +=
                                     "force = force + calculateRepulsion(p);\n"
                                     "force = force + calculateTargetMesh(p);\n";

                             if (macros.contains(NEED_PARTICLE_VECTOR_FIELD)) {
                                 source +=
                                         "force = force + calculateVectorField(p, uVectorFieldFactor,\n"
                                         "                    uVectorFieldTexture, uVectorFieldSampler);\n";
                             }

                             if (macros.contains(NEED_PARTICLE_CURL_NOISE)) {
                                 source += "force = force + calculateCurlNoise(p);\n";
                             }

                             // Integrations vectors.
                             source +=
                                     "        let dt = vec3<f32>(u_simulationData.timeStep);\n"
                                     "        var velocity = p.velocity.xyz;\n"
                                     "        var position = p.position.xyz;\n"
                                     "        \n"
                                     "        // Integrate velocity.\n"
                                     "        velocity = fma(force, dt, velocity);\n";

                             if (macros.contains(NEED_PARTICLE_VELOCITY_CONTROL)) {
                                 source += "velocity = u_simulationData.velocityFactor * normalize(velocity);\n";
                             }

                             // Integrate position.
                             source +=
                                     "        position = fma(velocity, dt, position);\n"
                                     "        \n"
                                     "        // Handle collisions.\n"
                                     "        collisionHandling(&position, &velocity);\n"
                                     "        \n"
                                     "        // Update the particle.\n"
                                     "        updateParticle(&p, position, velocity, age);\n"
                                     "        \n"
                                     "        // Save it in buffer.\n"
                                     "        pushParticle(p);\n"
                                     "    }\n";
                         },
                         {{"global_id", BuiltInType::GlobalInvocationID}});
        encoder.flush();
    }
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

}  // namespace vox
