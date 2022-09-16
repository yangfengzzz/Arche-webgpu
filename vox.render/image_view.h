//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/image.h"

namespace vox {
class ImageView {
public:
    explicit ImageView(Image* image,
                       wgpu::TextureViewDimension view_type,
                       uint32_t base_mip_level,
                       uint32_t base_array_layer,
                       uint32_t n_mip_levels,
                       uint32_t n_array_layers);

    [[nodiscard]] wgpu::TextureFormat format() const;

    [[nodiscard]] wgpu::TextureViewDimension dimension() const;

    [[nodiscard]] uint32_t baseMipLevel() const;

    [[nodiscard]] uint32_t mipLevelCount() const;

    [[nodiscard]] uint32_t baseArrayLayer() const;

    [[nodiscard]] uint32_t arrayLayerCount() const;

    [[nodiscard]] wgpu::TextureAspect aspect() const;

    [[nodiscard]] uint32_t sampleCount() const;

    const wgpu::TextureView& handle();
private:
    uint32_t _sampleCount{1};
    wgpu::TextureViewDescriptor _desc{};
    wgpu::TextureView _handle{};
};

}