//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/texture/sampled_texture2d.h"

namespace vox {
class SampledTexture2DView final : public SampledTexture2D {
public:
    SampledTexture2DView(wgpu::Device& device, std::function<wgpu::TextureView()> creator);

    wgpu::TextureView textureView() override;

private:
    std::function<wgpu::TextureView()> _creator;
};
using SampledTexture2DViewPtr = std::shared_ptr<SampledTexture2DView>;

}  // namespace vox