//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

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
