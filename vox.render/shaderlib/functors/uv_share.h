//
//  uv_share.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/29.
//

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
