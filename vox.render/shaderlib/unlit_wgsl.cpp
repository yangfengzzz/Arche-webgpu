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
    auto patch = WGSLPatchTest(this);
    patch(macros);

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
    
    begin(wgpu::ShaderStage::Fragment);
    beginInputStruct("FragmentInput");
    addInputType("@location(0) vColor: vec3<f32>;");
    endInputStruct();
    
    beginOutputStruct("Output");
    addOutputType("@location(0) finalColor: vec4<f32>;");
    endOutputStruct();
    
    beginEntry({"fragmentInput"});
    addEntry("var output: Output;\n "
             "output.finalColor = vec4<f32>(fragmentInput.vColor, 1.0);"
             "return output;\n ");
    endEntry();
    end();
    
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

}
