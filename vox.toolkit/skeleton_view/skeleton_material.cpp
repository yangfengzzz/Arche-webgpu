//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/skeleton_view/skeleton_material.h"

#include "vox.render/shader/shader_manager.h"

namespace vox {
const std::string BoneMaterial::_baseColorProp = "u_baseColor";
const std::string BoneMaterial::_skeletonBufferProp = "u_jointMatrix";
BoneMaterial::BoneMaterial(wgpu::Device& device, const std::string& name) : BaseMaterial(device, name) {
    vertex_source_ = ShaderManager::GetSingleton().LoadShader("toolkit/skeleton_view/bone.vert");
    fragment_source_ = ShaderManager::GetSingleton().LoadShader("toolkit/skeleton_view/skeleton.frag");
    shaderData.setData(_baseColorProp, Color(1, 1, 1, 1));
}

void BoneMaterial::setBaseColor(const Color& newValue) { shaderData.setData(BoneMaterial::_baseColorProp, newValue); }

void BoneMaterial::setSkeletonBuffer(const std::vector<float>& value) {
    shaderData.setData(BoneMaterial::_skeletonBufferProp, value);
}

const std::string JointMaterial::_skeletonBufferProp = "u_jointMatrix";
JointMaterial::JointMaterial(wgpu::Device& device, const std::string& name) : BaseMaterial(device, name) {
    vertex_source_ = ShaderManager::GetSingleton().LoadShader("toolkit/skeleton_view/joint.vert");
    fragment_source_ = ShaderManager::GetSingleton().LoadShader("toolkit/skeleton_view/skeleton.frag");
}

void JointMaterial::setSkeletonBuffer(const std::vector<float>& value) {
    shaderData.setData(JointMaterial::_skeletonBufferProp, value);
}

}  // namespace vox