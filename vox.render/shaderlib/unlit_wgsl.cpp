//
//  unlit_wgsl.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/27.
//

#include "unlit_wgsl.h"

namespace vox {
const std::string& UnlitVertexWGSL::compile(const ShaderMacroCollection& macros) {
    size_t hash = macros.hash();
    auto iter = _cache.find(hash);
    if (iter != _cache.end()) {
        return iter->second;
    } else {
        _cache[hash] = "";
        return _cache[hash];
    }
}

const std::string& UnlitFragmentWGSL::compile(const ShaderMacroCollection& macros) {
    size_t hash = macros.hash();
    auto iter = _cache.find(hash);
    if (iter != _cache.end()) {
        return iter->second;
    } else {
        _cache[hash] = "";
        return _cache[hash];
    }
}

WGSLPtr unlitVertex() {
    return std::make_unique<UnlitVertexWGSL>();
}

WGSLPtr fragmentVertex() {
    return std::make_unique<UnlitFragmentWGSL>();
}

}
