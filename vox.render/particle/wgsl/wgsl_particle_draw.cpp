//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_particle_draw.h"

namespace vox {
WGSLParticleVertex::WGSLParticleVertex() {
    
}
    
void WGSLParticleVertex::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source.clear();
    _bindGroupInfo.clear();
    auto inputStructCounter = WGSLEncoder::startCounter();
    auto outputStructCounter = WGSLEncoder::startCounter(0);
    {
        auto encoder = createSourceEncoder(wgpu::ShaderStage::Vertex);
        encoder.addInoutType("VertexIn", 0, "position", UniformType::Vec3f32);
        encoder.addInoutType("VertexIn", 1, "velocity", UniformType::Vec3f32);
        encoder.addInoutType("VertexIn", 2, "age_info", UniformType::Vec2f32);

        encoder.addInoutType("VertexOut", BuiltInType::Position, "position", UniformType::Vec4f32);
        encoder.addInoutType("VertexOut", BuiltInType::Position, "pointSize", UniformType::F32);

        encoder.addInoutType("VertexOut", BuiltInType::Position, "position", UniformType::Vec4f32);

        encoder.addEntry({{"in", "VertexIn"}}, {"out", "VertexOut"}, [&](std::string &source){

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
    
}

}
