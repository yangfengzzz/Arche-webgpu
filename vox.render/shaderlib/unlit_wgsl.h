//
//  unlit_wgsl.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/27.
//

#ifndef unlit_wgsl_hpp
#define unlit_wgsl_hpp

#include "wgsl.h"

namespace vox {
class UnlitVertexWGSL : public WGSL {
public:
    const std::string& compile(const ShaderMacroCollection& macros) override;
    
private:
    std::string cache;
};

class UnlitFragmentWGSL : public WGSL {
public:
    const std::string& compile(const ShaderMacroCollection& macros) override;
    
private:
    std::string cache;
};

}

#endif /* unlit_wgsl_hpp */
