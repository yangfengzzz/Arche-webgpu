//
//  wgsl_common.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/29.
//

#ifndef wgsl_common_hpp
#define wgsl_common_hpp

#include "wgsl_encoder.h"

namespace vox {
class WGSLCommon {
public:
    void operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros);
};

}

#endif /* wgsl_common_hpp */
