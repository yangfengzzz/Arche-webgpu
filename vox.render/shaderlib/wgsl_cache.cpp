//
//  wgsl_cache.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/29.
//

#include "wgsl_cache.h"

namespace vox {
std::pair<const std::string&, const WGSL::BindGroupInfo&> WGSLCache::compile(const ShaderMacroCollection& macros) {
    size_t hash = macros.hash();
    auto iter = _sourceCache.find(hash);
    if (iter == _sourceCache.end()) {
        _createShaderSource(hash, macros);
    }
    return {_sourceCache[hash], _infoCache[hash]};
}

}
