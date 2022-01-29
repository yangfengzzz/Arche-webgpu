//
//  wgsl_common_vert.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/29.
//

#ifndef wgsl_common_vert_hpp
#define wgsl_common_vert_hpp

#include "wgsl_encoder.h"

namespace vox {
class WGSLCommonVert {
public:
    WGSLCommonVert(const string& inputStructName);
    
    void operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros);
    
private:
    std::string _inputStructName{};
};

}

#endif /* wgsl_common_vert_hpp */
