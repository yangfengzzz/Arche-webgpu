//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/skeleton_view/skeleton_material.h"
#include "vox.render/shader/shader_manager.h"

namespace vox {
BoneMaterial::BoneMaterial(wgpu::Device& device, const std::string& name) : BaseMaterial(device, name) {
    vertex_source_ = ShaderManager::GetSingleton().LoadShader("blinn-phong.vert");
    fragment_source_ = ShaderManager::GetSingleton().LoadShader("blinn-phong.frag");
}

JointMaterial::JointMaterial(wgpu::Device& device, const std::string& name) : BaseMaterial(device, name) {
    vertex_source_ = ShaderManager::GetSingleton().LoadShader("blinn-phong.vert");
    fragment_source_ = ShaderManager::GetSingleton().LoadShader("blinn-phong.frag");
}
}