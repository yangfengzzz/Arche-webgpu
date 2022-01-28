//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef unlit_wgsl_hpp
#define unlit_wgsl_hpp

#include "wgsl.h"
#include <unordered_map>

namespace vox {
//MARK: - Unlit Vertex Code
class UnlitVertexWGSL : public WGSL {
public:
    UnlitVertexWGSL();
    
    std::pair<const std::string&, const BindGroupInfo&> compile(const ShaderMacroCollection& macros) override;
    
private:
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros);
    
    std::unordered_map<size_t, std::string> _sourceCache{};
    std::unordered_map<size_t, BindGroupInfo> _infoCache{};
};

//MARK: - Unlit Fragment Code
class UnlitFragmentWGSL : public WGSL {
public:
    UnlitFragmentWGSL();
    
    std::pair<const std::string&, const BindGroupInfo&> compile(const ShaderMacroCollection& macros) override;
    
private:
    void _createShaderSource(size_t hash, const ShaderMacroCollection& macros);
    
    std::unordered_map<size_t, std::string> _sourceCache{};
    std::unordered_map<size_t, BindGroupInfo> _infoCache{};
};

}

#endif /* unlit_wgsl_hpp */
