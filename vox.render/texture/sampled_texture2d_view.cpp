//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "sampled_texture2d_view.h"

namespace vox {
SampledTexture2DView::SampledTexture2DView(wgpu::Device& device, wgpu::TextureView&& view):
SampledTexture2D(device),
_view(view) {
    
}

wgpu::TextureView SampledTexture2DView::textureView() {
    return _view;
}

}
