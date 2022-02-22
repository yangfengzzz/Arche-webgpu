//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef shadow_material_hpp
#define shadow_material_hpp

#include "material/base_material.h"
#include "matrix4x4.h"

namespace vox {
class ShadowMaterial: public BaseMaterial {
public:
    /**
     * Create a ShadowMaterial instance.
     */
    ShadowMaterial(wgpu::Device& device);
    
    void setViewProjectionMatrix(const Matrix4x4F& vp);
    
    const Matrix4x4F& viewProjectionMatrix() const;

private:
    Matrix4x4F _vp = Matrix4x4F();
    ShaderProperty _shadowViewProjectionProp;
};

}

#endif /* shadow_material_hpp */
