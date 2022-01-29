//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "unlit_wgsl.h"

namespace vox {
//MARK: - Unlit Vertex Code
UnlitVertexWGSL::UnlitVertexWGSL() {}

std::pair<const std::string&, const WGSL::BindGroupInfo&> UnlitVertexWGSL::compile(const ShaderMacroCollection& macros) {
    size_t hash = macros.hash();
    auto iter = _sourceCache.find(hash);
    if (iter == _sourceCache.end()) {
        _createShaderSource(hash, macros);
    }
    return {_sourceCache[hash], _infoCache[hash]};
}

void UnlitVertexWGSL::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source = "";
    _bindGroupInfo.clear();
    
    auto encoder = createSourceEncoder(wgpu::ShaderStage::Vertex);
    encoder.addUniformBinding("u_projMat", UniformType::Mat4x4f32);
    encoder.addUniformBinding("u_MVMat", UniformType::Mat4x4f32);
    encoder.addInputType("VertexInput", "@location(0) aVertexPosition: vec3<f32>;\n");
    encoder.addInputType("VertexInput", "@location(1) aVertexNormal: vec3<f32>;\n");
    encoder.addInputType("VertexInput", "@location(2) aVertexUV: vec2<f32>;\n");
    encoder.addOutputType("Output", "@location(0) vColor: vec3<f32>;\n");
    encoder.addOutputType("Output", "@builtin(position) Position: vec4<f32>;");
    encoder.addEntry({"vertexInput"}, []()->auto{
        return
        "var output: Output;\n "
        "output.Position = u_projMat * u_MVMat * vec4<f32>(vertexInput.aVertexPosition, 1.0);\n "
        "output.vColor = vertexInput.aVertexPosition;\n "
        "return output;\n ";
    });
    
    
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

//MARK: - Unlit Fragment Code
UnlitFragmentWGSL::UnlitFragmentWGSL(){}

std::pair<const std::string&, const WGSL::BindGroupInfo&> UnlitFragmentWGSL::compile(const ShaderMacroCollection& macros) {
    size_t hash = macros.hash();
    auto iter = _sourceCache.find(hash);
    if (iter == _sourceCache.end()) {
        _createShaderSource(hash, macros);
    }
    return {_sourceCache[hash], _infoCache[hash]};
}

void UnlitFragmentWGSL::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source = "";
    _bindGroupInfo.clear();
    
    auto encoder = createSourceEncoder(wgpu::ShaderStage::Fragment);
    encoder.addInputType("FragmentInput", "@location(0) vColor: vec3<f32>;");
    encoder.addOutputType("Output", "@location(0) finalColor: vec4<f32>;");
    encoder.addEntry({"fragmentInput"}, []()->auto{
        return
        "var output: Output;\n "
        "output.finalColor = vec4<f32>(fragmentInput.vColor, 1.0);"
        "return output;\n ";
    });
    encoder.flush();
    
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

}
