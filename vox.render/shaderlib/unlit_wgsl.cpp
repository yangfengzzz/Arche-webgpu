//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "unlit_wgsl.h"
#include "wgsl_library.h"

namespace vox {
//MARK: - Unlit Vertex Code
UnlitVertexWGSL::UnlitVertexWGSL() {}

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
    _source = "";
    auto patch = WGSLPatchTest(this);
    patch(macros);

    _cache[hash] = _source;
}

//MARK: - Unlit Fragment Code
UnlitFragmentWGSL::UnlitFragmentWGSL(){}

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
    "@stage(fragment)\n"
    "fn main(@location(0) vColor: vec3<f32>) ->  @location(0) vec4<f32> {\n"
    "  return vec4<f32>(vColor, 1.0);\n"
    "}\n";
    _cache[hash] = std::move(source);
}

}
