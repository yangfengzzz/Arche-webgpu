
//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/shadow/shadow_material.h"

#include "vox.render/shader/shader_manager.h"

namespace vox {
ShadowMaterial::ShadowMaterial(wgpu::Device& device) : BaseMaterial(device) {
    vertex_source_ = ShaderManager::GetSingleton().LoadShader("shadow/shadow-map.vert");
}
}  // namespace vox