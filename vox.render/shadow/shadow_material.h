//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/matrix4x4.h"
#include "vox.render/material/base_material.h"

namespace vox {
class ShadowMaterial : public BaseMaterial {
public:
    /**
     * Create a ShadowMaterial instance.
     */
    explicit ShadowMaterial(wgpu::Device& device);

    void setViewProjectionMatrix(const Matrix4x4F& vp);

    [[nodiscard]] const Matrix4x4F& viewProjectionMatrix() const;

private:
    Matrix4x4F _vp = Matrix4x4F();
    ShaderProperty _shadowViewProjectionProp;
};

}  // namespace vox