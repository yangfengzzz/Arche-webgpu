//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

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
