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
    encoder.addInoutType("VertexInput", Attributes::Position, UniformType::Vec3f32);
    encoder.addInoutType("VertexInput", Attributes::Normal, UniformType::Vec3f32);
    encoder.addInoutType("VertexInput", Attributes::UV_0, UniformType::Vec2f32);
    encoder.addInoutType("Output", 0, "vColor", UniformType::Vec3f32);
    encoder.addInoutType("Output", BuiltInType::Position, "Position", UniformType::Vec4f32);
    encoder.addEntry({{"vertexInput", "VertexInput"}}, "Output",  [](std::string& source) {
        source +=
        "var output: Output;\n "
        "output.Position = u_projMat * u_MVMat * vec4<f32>(vertexInput.Position, 1.0);\n "
        "output.vColor = vertexInput.Position;\n "
        "return output;\n ";
    });
    encoder.flush();
    
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
    encoder.addInoutType("FragmentInput", 0, "vColor", UniformType::Vec3f32);
    encoder.addInoutType("Output", 0, "finalColor", UniformType::Vec4f32);
    encoder.addEntry({{"fragmentInput", "FragmentInput"}}, "Output", [](std::string& source) {
        source +=
        "var output: Output;\n "
        "output.finalColor = vec4<f32>(fragmentInput.vColor, 1.0);"
        "return output;\n ";
    });
    encoder.flush();
    
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

}
