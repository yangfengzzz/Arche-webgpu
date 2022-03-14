//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_particle_draw.h"
#include <fmt/core.h>

namespace vox {
WGSLParticleVertex::WGSLParticleVertex():
_commonFrag("VertexIn") {
}
    
void WGSLParticleVertex::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source.clear();
    _bindGroupInfo.clear();
    auto inputStructCounter = WGSLEncoder::startCounter();
    auto outputStructCounter = WGSLEncoder::startCounter(0);
    {
        auto encoder = createSourceEncoder(wgpu::ShaderStage::Vertex);
        encoder.addStruct("var<private> pos : array<vec2<f32>, 4> = array<vec2<f32>, 4>(\n"
                          "  vec2<f32>(-1.0, 1.0), vec2<f32>(1.0, 1.0), vec2<f32>(-1.0, -1.0), vec2<f32>(1.0, -1.0)\n"
                          ");\n");
        
        encoder.addStruct("struct ParticleData {\n"
                          "    birthGradient: vec3<f32>;\n"
                          "    minParticleSize: f32;\n"
                          "    deathGradient: vec3<f32>;\n"
                          "    maxParticleSize: f32;\n"
                          "    colorMode: u32;\n"
                          "    fadeCoefficient: f32;\n"
                          "    debugDraw: bool;\n"
                          "};\n");
        encoder.addUniformBinding("u_particleData", "ParticleData");
        
        auto particleCount = *macros.macroConstant(PARTICLE_COUNT);
        encoder.addStorageBufferBinding("u_writeConsumeBuffer", fmt::format("array<TParticle, {}>", particleCount), false);
        
        _commonFrag(encoder, macros);
        
        encoder.addInoutType("VertexIn", BuiltInType::InstanceIndex, "instanceIndex", UniformType::U32);
        encoder.addInoutType("VertexIn", BuiltInType::VertexIndex, "vertexIndex", UniformType::U32);

        encoder.addInoutType("VertexOut", BuiltInType::Position, "position", UniformType::Vec4f32);
        encoder.addInoutType("VertexOut", 0, "uv", UniformType::Vec2f32);
        encoder.addInoutType("VertexOut", 1, "color", UniformType::Vec3f32);
        encoder.addInoutType("VertexOut", 2, "decay", UniformType::F32);

        encoder.addEntry({{"in", "VertexIn"}}, {"out", "VertexOut"}, [&](std::string &source){
            source +=
            "    var out:VertexOut;\n"
            "    \n"
            "    // Time alived in [0, 1].\n"
            "    let dAge = 1.0f - maprange(0.0f, u_writeConsumeBuffer[instanceIndex].start_age,\n"
            "                                       u_writeConsumeBuffer[instanceIndex].age);\n"
            "    let decay = curve_inout(dAge, 0.55f);\n"
            "    \n"
            "    out.uv = pos[vertexIndex];\n"
            "    let worldPos = vec3<f32>(out.uv, 0.0) * compute_size(out.position.z/out.position.w, decay,\n"
            "                                                         uMinParticleSize, uMaxParticleSize) * 0.025;\n"
            "    \n"
            "    // Generate a billboarded model view matrix\n"
            "    var bbModelViewMatrix:mat4x4<f32>;\n"
            "    bbModelViewMatrix[3] = float4(u_writeConsumeBuffer[instanceIndex].position.xyz, 1.0);\n"
            "    bbModelViewMatrix = u_cameraData.u_viewMat * bbModelViewMatrix;\n"
            "    bbModelViewMatrix[0][0] = 1.0;\n"
            "    bbModelViewMatrix[0][1] = 0.0;\n"
            "    bbModelViewMatrix[0][2] = 0.0;\n"
            "\n"
            "    bbModelViewMatrix[1][0] = 0.0;\n"
            "    bbModelViewMatrix[1][1] = 1.0;\n"
            "    bbModelViewMatrix[1][2] = 0.0;\n"
            "\n"
            "    bbModelViewMatrix[2][0] = 0.0;\n"
            "    bbModelViewMatrix[2][1] = 0.0;\n"
            "    bbModelViewMatrix[2][2] = 1.0;\n"
            "    out.position = u_cameraData.u_projMat * bbModelViewMatrix * float4(worldPos, 1.0);\n"
            "    \n"
            "    // Output parameters.\n"
            "    out.color = base_color(uWriteParticle[instanceIndex].position.xyz, decay,\n"
            "                           uColorMode, uBirthGradient, uDeathGradient);\n"
            "    out.decay = decay;\n"
            "    \n"
            "    return out;\n";
        });
        encoder.flush();
    }
    WGSLEncoder::endCounter(inputStructCounter);
    WGSLEncoder::endCounter(outputStructCounter);
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

//MARK: - Particle Fragment Code
WGSLParticleFragment::WGSLParticleFragment() {
    
}
    
void WGSLParticleFragment::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source.clear();
    _bindGroupInfo.clear();
    auto inputStructCounter = WGSLEncoder::startCounter(0);
    {
        auto encoder = createSourceEncoder(wgpu::ShaderStage::Fragment);
        
        encoder.addStruct("struct ParticleData {\n"
                          "    birthGradient: vec3<f32>;\n"
                          "    minParticleSize: f32;\n"
                          "    deathGradient: vec3<f32>;\n"
                          "    maxParticleSize: f32;\n"
                          "    colorMode: u32;\n"
                          "    fadeCoefficient: f32;\n"
                          "    debugDraw: bool;\n"
                          "};\n");
        encoder.addUniformBinding("u_particleData", "ParticleData");
        
        encoder.addInoutType("VertexOut", 0, "uv", UniformType::Vec2f32);
        encoder.addInoutType("VertexOut", 1, "color", UniformType::Vec3f32);
        encoder.addInoutType("VertexOut", 2, "decay", UniformType::F32);

        encoder.addInoutType("Output", 0, "finalColor", UniformType::Vec4f32);
        encoder.addEntry({{"in", "VertexOut"}}, {"out", "Output"},  [&](std::string &source){
            source += "out.finalColor = compute_color(in.color, in.decay, in.uv, uFadeCoefficient, uDebugDraw);\n";
        });
        encoder.flush();
    }
    WGSLEncoder::endCounter(inputStructCounter);
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

}
