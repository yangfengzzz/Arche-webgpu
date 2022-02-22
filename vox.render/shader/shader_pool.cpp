//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shader_pool.h"
#include "shader.h"

#include "shaderlib/wgsl_unlit.h"
#include "shaderlib/wgsl_blinn_phong.h"
#include "shaderlib/wgsl_pbr.h"
#include "shadow/wgsl_shadow.h"

namespace vox {
void ShaderPool::init() {
    Shader::create("unlit", std::make_unique<WGSLUnlitVertex>(), std::make_unique<WGSLUnlitFragment>());
    Shader::create("blinn-phong", std::make_unique<WGSLBlinnPhongVertex>(), std::make_unique<WGSLBlinnPhongFragment>());
    Shader::create("pbr", std::make_unique<WGSLPbrVertex>(), std::make_unique<WGSLPbrFragment>(true));
    Shader::create("pbr-specular", std::make_unique<WGSLPbrVertex>(), std::make_unique<WGSLPbrFragment>(false));
    
    Shader::create("shadow", std::make_unique<WGSLShadowVertex>(), nullptr);
}

}

