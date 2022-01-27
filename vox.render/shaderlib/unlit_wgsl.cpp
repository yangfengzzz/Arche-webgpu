//
//  unlit_wgsl.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/27.
//

#include "unlit_wgsl.h"

namespace vox {
const std::string& UnlitVertexWGSL::compile(const ShaderMacroCollection& macros) {
    return cache;
}

const std::string& UnlitFragmentWGSL::compile(const ShaderMacroCollection& macros) {
    return cache;
}

}
