//
//  wgsl_common_vert.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/29.
//

#ifndef wgsl_common_vert_hpp
#define wgsl_common_vert_hpp

#include "wgsl_library.h"

namespace vox {
class WGSLCommonVert {
public:
    WGSLCommonVert(WGSL* source);
    
    void operator()(const ShaderMacroCollection& macros);
    
private:
    WGSLUniformBinding _uPMatirx;
    WGSLUniformBinding _uMVMatrix;
    
    WGSL* _source{nullptr};
};

}

#endif /* wgsl_common_vert_hpp */
