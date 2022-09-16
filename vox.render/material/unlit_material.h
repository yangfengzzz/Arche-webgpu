//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/color.h"
#include "vox.render/material/base_material.h"
#include "vox.render/image.h"

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
    [[nodiscard]] std::shared_ptr<Image> baseTexture() const;

    void setBaseTexture(const std::shared_ptr<Image>& newValue);

    void setBaseSampler(const wgpu::SamplerDescriptor &desc);

    /**
     * Create a unlit material instance.
     */
    explicit UnlitMaterial(wgpu::Device& device, const std::string& name = "");

private:
    Color _baseColor = Color(1, 1, 1, 1);
    static const std::string _baseColorProp;

    std::shared_ptr<Image> _baseTexture = nullptr;
    static const std::string _baseTextureProp;
    static const std::string _baseSamplerProp;
};

}  // namespace vox