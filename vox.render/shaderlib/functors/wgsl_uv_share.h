//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef uv_share_hpp
#define uv_share_hpp

#include "wgsl_encoder.h"

namespace vox {
class WGSLUVShare {
public:
    WGSLUVShare(const std::string& outputStructName, size_t counterIndex);
    
    void operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros);
    
private:
    std::string _outputStructName{};
    size_t _counterIndex{};
};

}
#endif /* uv_share_hpp */
