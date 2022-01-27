//
//  unlit_wgsl.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/27.
//

#include "unlit_wgsl.h"

namespace vox {
//MARK: - Unlit Vertex Code
const std::string& UnlitVertexWGSL::compile(const ShaderMacroCollection& macros) {
    size_t hash = macros.hash();
    auto iter = _cache.find(hash);
    if (iter != _cache.end()) {
        return iter->second;
    } else {
        _createShaderSource(hash, macros);
        return _cache[hash];
    }
}

void UnlitVertexWGSL::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    std::string source = "\n"
    "  [[block]]struct uPMatrixUniforms {\n"
    "        [[size(64)]]m: mat4x4<f32>;\n"
    "  };\n"
    "\n"
    "  [[block]]struct uMVMatrixUniforms {\n"
    "    [[size(64)]]m: mat4x4<f32>;\n"
    "  };\n"
    "\n"
    "  [[group(0), binding(0)]]\n"
    "  var<uniform> uPMatrix: uPMatrixUniforms;\n"
    "  [[group(0), binding(1)]]\n"
    "  var<uniform> uMVMatrix: uMVMatrixUniforms;\n"
    "\n"
    "  struct VertexInput {\n"
    "      [[location(0)]] aVertexPosition: vec3<f32>;\n"
    "      [[location(1)]] aVertexNormal: vec3<f32>;\n"
    "      [[location(2)]] aVertexUV: vec2<f32>;\n"
    "  };\n"
    "\n"
    "  struct Output {\n"
    "    [[location(0)]] vColor: vec3<f32>;\n"
    "    [[builtin(position)]] Position: vec4<f32>;\n"
    "  };\n"
    "\n"
    "  [[stage(vertex)]]\n"
    "  fn main(vertexInput: VertexInput) -> Output {\n"
    "    var output: Output;\n"
    "    output.Position = uPMatrix.m * uMVMatrix.m * vec4<f32>(vertexInput.aVertexPosition, 1.0);\n"
    "    output.vColor = vertexInput.aVertexPosition;\n"
    "    return output;\n"
    "  } \n";
    _cache[hash] = std::move(source);
}


WGSLPtr unlitVertex() {
    return std::make_unique<UnlitVertexWGSL>();
}

//MARK: - Unlit Fragment Code
const std::string& UnlitFragmentWGSL::compile(const ShaderMacroCollection& macros) {
    size_t hash = macros.hash();
    auto iter = _cache.find(hash);
    if (iter != _cache.end()) {
        return iter->second;
    } else {
        _createShaderSource(hash, macros);
        return _cache[hash];
    }
}

void UnlitFragmentWGSL::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    std::string source = "\n"
    "[[stage(fragment)]]\n"
    "fn main([[location(0)]] vColor: vec3<f32>) ->  [[location(0)]] vec4<f32> {\n"
    "  return vec4<f32>(vColor, 1.0);\n"
    "}\n";
    _cache[hash] = std::move(source);
}

WGSLPtr unlitFragment() {
    return std::make_unique<UnlitFragmentWGSL>();
}

}
