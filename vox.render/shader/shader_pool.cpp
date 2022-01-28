//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shader_pool.h"
#include "shader.h"

#include "shaderlib/unlit_wgsl.h"

namespace vox {
void ShaderPool::init() {
    Shader::create("unlit", std::make_unique<UnlitVertexWGSL>(), std::make_unique<UnlitFragmentWGSL>());
}

}

