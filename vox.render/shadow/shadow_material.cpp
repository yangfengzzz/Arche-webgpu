//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/shadow/shadow_material.h"

namespace vox {
ShadowMaterial::ShadowMaterial(wgpu::Device& device)
    : BaseMaterial(device), _shadowViewProjectionProp("u_shadowVPMat") {}

void ShadowMaterial::setViewProjectionMatrix(const Matrix4x4F& vp) {
    _vp = vp;
    shaderData.setData(_shadowViewProjectionProp, _vp);
}

const Matrix4x4F& ShadowMaterial::viewProjectionMatrix() const { return _vp; }

}  // namespace vox
