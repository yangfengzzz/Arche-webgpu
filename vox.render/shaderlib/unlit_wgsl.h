//
//  unlit_wgsl.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/27.
//

#ifndef unlit_wgsl_hpp
#define unlit_wgsl_hpp

#include "wgsl.h"
#include <unordered_map>

namespace vox {
//MARK: - Unlit Vertex Code
class UnlitVertexWGSL : public WGSL {
public:
    const std::string& compile(const ShaderMacroCollection& macros) override;
    
private:
    std::unordered_map<size_t, std::string> _cache{};
};

WGSLPtr unlitVertex();

//MARK: - Unlit Fragment Code
class UnlitFragmentWGSL : public WGSL {
public:
    const std::string& compile(const ShaderMacroCollection& macros) override;
    
private:
    std::unordered_map<size_t, std::string> _cache{};
};

WGSLPtr unlitFragment();

}

#endif /* unlit_wgsl_hpp */
