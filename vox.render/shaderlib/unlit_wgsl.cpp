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
    
}

WGSLPtr fragmentVertex() {
    return std::make_unique<UnlitFragmentWGSL>();
}

}
