//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/shadow/shadow_debug_material.h"

#include "vox.render/shaderlib/wgsl_blinn_phong.h"
#include "vox.render/shadow/wgsl/wgsl_shadow_debug.h"

namespace vox {
ShadowDebugMaterial::ShadowDebugMaterial(wgpu::Device& device)
    : BaseMaterial(device,
                   Shader::create("shadow_debug_material",
                                  std::make_unique<WGSLBlinnPhongVertex>(),
                                  std::make_unique<WGSLShadowDebug>())) {}

}  // namespace vox
