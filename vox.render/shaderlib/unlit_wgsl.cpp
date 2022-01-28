//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "unlit_wgsl.h"
#include "wgsl_library.h"
#include <fmt/core.h>

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
    std::vector<std::string> uniform{};
    std::vector<std::string> input{};
    std::vector<std::string> output{};
    std::vector<std::string> entry{};
    std::vector<std::string> function{};
    auto patch = WGSLPatchTest(uniform, input, output, entry, function);
    patch();
    
    std::string uniformString{};
    for (const auto& string : uniform) {
        uniformString += string;
    }
    std::string inputString{};
    for (const auto& string : input) {
        inputString += string;
    }
    std::string outputString{};
    for (const auto& string : output) {
        outputString += string;
    }
    std::string entryString{};
    for (const auto& string : entry) {
        entryString += string;
    }
    std::string functionString{};
    for (const auto& string : function) {
        functionString += string;
    }
    
    const std::string sourceTemplate =
    "{0}\n "
    "\n "
    "struct VertexInput {{\n "
    "{1}\n "
    "}};\n "
    "\n "
    "struct Output {{\n "
    "{2}\n "
    "}};\n "
    "\n "
    "@stage(vertex)\n"
    "fn main(vertexInput: VertexInput) -> Output {{\n"
    "{3}\n"
    "}}\n";
    _cache[hash] = fmt::format(sourceTemplate, uniformString,
                               inputString, outputString, entryString, functionString);
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
    "@stage(fragment)\n"
    "fn main(@location(0) vColor: vec3<f32>) ->  @location(0) vec4<f32> {\n"
    "  return vec4<f32>(vColor, 1.0);\n"
    "}\n";
    _cache[hash] = std::move(source);
}

WGSLPtr unlitFragment() {
    return std::make_unique<UnlitFragmentWGSL>();
}

}
