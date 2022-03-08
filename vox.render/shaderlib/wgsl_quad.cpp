//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_quad.h"

namespace vox {
void WGSLQuadVertex::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source.clear();
    _bindGroupInfo.clear();
    auto inputStructCounter = WGSLEncoder::startCounter();
    auto outputStructCounter = WGSLEncoder::startCounter(0);
    {
        auto encoder = createSourceEncoder(wgpu::ShaderStage::Vertex);
        encoder.addInoutType("VertexIn", BuiltInType::VertexIndex, "vertex_index", UniformType::U32);
        encoder.addStruct("let quadVertex = array<vec4<f32>, 3>(vec4<f32>(-1.0, -1.0, 0.0, 1.0), vec4<f32>(-1.0, 1.0, 0.0, 1.0), vec4<f32>(1.0, 1.0, 0.0, 1.0));\n");
        encoder.addInoutType("VertexOut", BuiltInType::Position, "position", UniformType::Vec4f32);

        encoder.addEntry({{"in", "VertexIn"}}, {"out", "VertexOut"}, [&](std::string &source){
            source += "out.position = quadVertex[in.vertex_index];\n";
        });
        encoder.flush();
    }
    WGSLEncoder::endCounter(inputStructCounter);
    WGSLEncoder::endCounter(outputStructCounter);
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

}
