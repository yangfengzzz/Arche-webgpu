//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/image_view.h"

namespace vox {
ImageView::ImageView(const Image* image,
                     wgpu::TextureViewDimension view_type,
                     uint32_t base_mip_level,
                     uint32_t base_array_layer,
                     uint32_t n_mip_levels,
                     uint32_t n_array_layers) {
    _desc.label = image->name.c_str();
    _desc.format = image->format();
    _desc.dimension = view_type;
    _desc.baseMipLevel = base_mip_level;
    _desc.baseArrayLayer = base_array_layer;
    _desc.mipLevelCount = n_mip_levels == 0 ? static_cast<uint32_t>(image->mipmaps().size()) : n_mip_levels;
    _desc.arrayLayerCount = n_array_layers == 0 ? image->layers() : n_array_layers;
    _handle = image->getTexture().CreateView(&_desc);
    _sampleCount = image->getTexture().GetSampleCount();
}

wgpu::TextureFormat ImageView::format() const { return _desc.format; }

wgpu::TextureViewDimension ImageView::dimension() const { return _desc.dimension; }

uint32_t ImageView::baseMipLevel() const { return _desc.baseMipLevel; }

uint32_t ImageView::mipLevelCount() const { return _desc.mipLevelCount; }

uint32_t ImageView::baseArrayLayer() const { return _desc.baseArrayLayer; }

uint32_t ImageView::arrayLayerCount() const { return _desc.arrayLayerCount; }

wgpu::TextureAspect ImageView::aspect() const { return _desc.aspect; }

uint32_t ImageView::sampleCount() const { return _sampleCount; }

const wgpu::TextureView& ImageView::handle() { return _handle; }

}  // namespace vox
