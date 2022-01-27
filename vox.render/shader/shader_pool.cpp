//
//  shader_pool.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/27.
//

#include "shader_pool.h"
#include "shader.h"

#include "shaderlib/unlit_wgsl.h"

namespace vox {
void ShaderPool::init() {
    Shader::create("unlit", unlitVertex, unlitFragment);
}

}

