//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef unlit_material_hpp
#define unlit_material_hpp

#include "base_material.h"
#include "texture/sampled_texture2d.h"
#include "vox.math/color.h"

namespace vox {
/**
 * Unlit Material.
 */
class UnlitMaterial : public BaseMaterial {
public:
    /**
     * Base color.
     */
    const Color& baseColor();

    void setBaseColor(const Color& newValue);

    /**
     * Base texture.
     */
    SampledTexture2DPtr baseTexture();

    void setBaseTexture(SampledTexture2DPtr newValue);

    /**
     * Create a unlit material instance.
     */
    explicit UnlitMaterial(wgpu::Device& device);

private:
    Color _baseColor = Color(1, 1, 1, 1);
    ShaderProperty _baseColorProp;

    SampledTexture2DPtr _baseTexture = nullptr;
    ShaderProperty _baseTextureProp;
    ShaderProperty _baseSamplerProp;
};

}  // namespace vox

#endif /* unlit_material_hpp */
