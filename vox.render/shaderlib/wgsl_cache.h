//
//  wgsl_cache.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/29.
//

#ifndef wgsl_cache_hpp
#define wgsl_cache_hpp

#include "wgsl.h"
#include <unordered_map>

namespace vox {
//MARK: - Unlit Vertex Code
class WGSLCache : public WGSL {
public:
    WGSLCache() = default;
    
    std::pair<const std::string&, const BindGroupInfo&> compile(const ShaderMacroCollection& macros) override;
    
protected:
    virtual void _createShaderSource(size_t hash, const ShaderMacroCollection& macros) = 0;
    
    std::unordered_map<size_t, std::string> _sourceCache{};
    std::unordered_map<size_t, BindGroupInfo> _infoCache{};
};

}
#endif /* wgsl_cache_hpp */
