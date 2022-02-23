//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef wgsl_shadow_frag_hpp
#define wgsl_shadow_frag_hpp

#include "shaderlib/wgsl_encoder.h"

namespace vox {
class WGSLShadowFrag {
public:
    WGSLShadowFrag(const std::pair<std::string, std::string>& outType);
    
    void operator()(WGSLEncoder& encoder,
                    const ShaderMacroCollection& macros, size_t counterIndex);
    
    void operator()(std::string& source, const ShaderMacroCollection& macros);
    
private:
    const std::string _output;
    const std::string _outputStructName{};
    
    std::string _shadowDataStruct{};
    std::string _cubeShadowDataStruct{};
};

}
#endif /* wgsl_shadow_frag_hpp */
