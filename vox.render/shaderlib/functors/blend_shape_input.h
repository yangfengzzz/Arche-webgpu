//
//  blend_shape_input.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/29.
//

#ifndef blend_shape_input_hpp
#define blend_shape_input_hpp

#include "wgsl_encoder.h"

namespace vox {
class WGSLBlendShapeInput {
public:
    WGSLBlendShapeInput(const std::string& inputStructName);
    
    void operator()(WGSLEncoder& encoder, const ShaderMacroCollection& macros);
    
private:
    std::string _inputStructName{};
};

}

#endif /* blend_shape_input_hpp */
