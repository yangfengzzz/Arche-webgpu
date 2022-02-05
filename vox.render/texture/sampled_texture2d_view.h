//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef sampled_texture2d_view_hpp
#define sampled_texture2d_view_hpp

#include "sampled_texture2d.h"

namespace vox {
class SampledTexture2DView final : public SampledTexture2D {
public:
    SampledTexture2DView(wgpu::Device& device, wgpu::TextureView& view);
    
    wgpu::TextureView textureView() override;
    
private:
    wgpu::TextureView& _view;
};
using SampledTexture2DViewPtr = std::shared_ptr<SampledTexture2DView>;

}

#endif /* sampled_texture2d_view_hpp */
