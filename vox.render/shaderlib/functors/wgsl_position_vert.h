//
//  wgsl_position_vert.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/29.
//

#ifndef wgsl_position_vert_hpp
#define wgsl_position_vert_hpp

#include "wgsl_encoder.h"

namespace vox {
class WGSLPositionVert {
public:
    void operator()(std::string& source, const ShaderMacroCollection& macros);
};

}

#endif /* wgsl_position_vert_hpp */
