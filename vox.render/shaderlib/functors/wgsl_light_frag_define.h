//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef wgsl_light_frag_define_hpp
#define wgsl_light_frag_define_hpp

#include "wgsl_encoder.h"

namespace vox {
class WGSLLightFragDefine {
public:
    WGSLLightFragDefine(const std::string& outputStructName);
    
    void operator()(WGSLEncoder& encoder,
                    const ShaderMacroCollection& macros, size_t counterIndex);
    
private:
    std::string _outputStructName{};
};

}

#endif /* wgsl_light_frag_define_hpp */
