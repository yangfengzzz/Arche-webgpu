//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/shader/shader_pool.h"

#include "vox.render/shader/shader.h"
#include "vox.render/shaderlib/wgsl_blinn_phong.h"
#include "vox.render/shaderlib/wgsl_pbr.h"
#include "vox.render/shaderlib/wgsl_unlit.h"
#include "vox.render/shadow/wgsl_shadow.h"

namespace vox {
void ShaderPool::init() {
    Shader::create("unlit", std::make_unique<WGSLUnlitVertex>(), std::make_unique<WGSLUnlitFragment>());
    Shader::create("blinn-phong", std::make_unique<WGSLBlinnPhongVertex>(), std::make_unique<WGSLBlinnPhongFragment>());
    Shader::create("pbr", std::make_unique<WGSLPbrVertex>(), std::make_unique<WGSLPbrFragment>(true));
    Shader::create("pbr-specular", std::make_unique<WGSLPbrVertex>(), std::make_unique<WGSLPbrFragment>(false));

    Shader::create("shadow", std::make_unique<WGSLShadowVertex>(), nullptr);
}

}  // namespace vox
