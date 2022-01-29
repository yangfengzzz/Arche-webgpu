//
//  wgsl_blend_shape_vert.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/29.
//

#ifndef wgsl_blend_shape_vert_hpp
#define wgsl_blend_shape_vert_hpp

#include "wgsl_encoder.h"

namespace vox {
class WGSLBlendShapeVert {
public:
    void operator()(std::string& source, const ShaderMacroCollection& macros);
};

}

#endif /* wgsl_blend_shape_vert_hpp */
