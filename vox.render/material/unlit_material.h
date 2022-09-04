//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/color.h"
#include "vox.render/material/base_material.h"
#include "vox.render/texture/sampled_texture2d.h"

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

    void setBaseTexture(const SampledTexture2DPtr& newValue);

    /**
     * Create a unlit material instance.
     */
    explicit UnlitMaterial(wgpu::Device& device, const std::string& name = "");

private:
    Color _baseColor = Color(1, 1, 1, 1);
    const std::string _baseColorProp;

    SampledTexture2DPtr _baseTexture = nullptr;
    const std::string _baseTextureProp;
    const std::string _baseSamplerProp;
};

}  // namespace vox