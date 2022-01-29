//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef wgsl_skinning_vert_hpp
#define wgsl_skinning_vert_hpp

#include "wgsl_encoder.h"

namespace vox {
class WGSLSkinningVert {
public:
    void operator()(std::string& source, const ShaderMacroCollection& macros);
};

}

#endif /* wgsl_skinning_vert_hpp */
