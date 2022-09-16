//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/image.h"

#include "vox.render/helper.h"
#include "vox.render/image_view.h"
#include "vox.render/platform/filesystem.h"
#include "vox.render/std_helpers.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>

#include "vox.render/image/astc_img.h"
#include "vox.render/image/ktx_img.h"
#include "vox.render/image/stb_img.h"

namespace vox {
Image::Image(std::string name, std::vector<uint8_t> &&d, std::vector<Mipmap> &&m)
    : name{std::move(name)}, _data{std::move(d)}, _format{wgpu::TextureFormat::RGBA8Unorm}, _mipmaps{std::move(m)} {}

const std::vector<uint8_t> &Image::data() const { return _data; }

void Image::clear() {
    _data.clear();
    _data.shrink_to_fit();
}

wgpu::TextureFormat Image::format() const { return _format; }

const wgpu::Extent3D &Image::extent() const { return _mipmaps.at(0).extent; }

uint32_t Image::layers() const { return _mipmaps.at(0).extent.depthOrArrayLayers; }

const std::vector<Mipmap> &Image::mipmaps() const { return _mipmaps; }

const std::vector<std::vector<uint64_t>> &Image::offsets() const { return _offsets; }

void Image::createTexture(wgpu::Device &device, wgpu::TextureUsage usage) {
    auto desc = wgpu::TextureDescriptor();
    desc.label = name.c_str();
    desc.usage = usage;
    desc.format = _format;
    desc.size = _mipmaps.at(0).extent;
    desc.mipLevelCount = _mipmaps.size();
}

const wgpu::Texture &Image::getTexture() const { return _texture; }

const std::shared_ptr<ImageView> &Image::getImageView(wgpu::TextureViewDimension view_type,
                                                      uint32_t base_mip_level,
                                                      uint32_t base_array_layer,
                                                      uint32_t n_mip_levels,
                                                      uint32_t n_array_layers) {
    std::size_t key = 0;
    vox::utility::hash_combine(key, view_type);
    vox::utility::hash_combine(key, base_mip_level);
    vox::utility::hash_combine(key, base_array_layer);
    vox::utility::hash_combine(key, n_mip_levels);
    vox::utility::hash_combine(key, n_array_layers);
    auto iter = _image_views.find(key);
    if (iter == _image_views.end()) {
        _image_views.insert(
                std::make_pair(key, std::make_shared<ImageView>(this, view_type, base_mip_level, base_array_layer,
                                                                n_mip_levels, n_array_layers)));
    }
    return _image_views.find(key)->second;
}

Mipmap &Image::mipmap(const size_t index) { return _mipmaps.at(index); }

void Image::generateMipmaps() {
    assert(_mipmaps.size() == 1 && "Mipmaps already generated");

    if (_mipmaps.size() > 1) {
        return;  // Do not generate again
    }

    const wgpu::Extent3D &extent = this->extent();
    auto next_width = std::max<uint32_t>(1u, extent.width / 2);
    auto next_height = std::max<uint32_t>(1u, extent.height / 2);
    auto channels = 4;
    auto next_size = next_width * next_height * channels;

    while (true) {
        // Make space for next mipmap
        auto old_size = to_u32(_data.size());
        _data.resize(old_size + next_size);

        auto &prev_mipmap = _mipmaps.back();
        // Update mipmaps
        Mipmap next_mipmap{};
        next_mipmap.level = prev_mipmap.level + 1;
        next_mipmap.offset = old_size;
        next_mipmap.extent = {next_width, next_height, 1u};

        // Fill next mipmap memory
        stbir_resize_uint8(_data.data() + prev_mipmap.offset, prev_mipmap.extent.width, prev_mipmap.extent.height, 0,
                           _data.data() + next_mipmap.offset, next_mipmap.extent.width, next_mipmap.extent.height, 0,
                           channels);

        _mipmaps.emplace_back(next_mipmap);

        // Next mipmap values
        next_width = std::max<uint32_t>(1u, next_width / 2);
        next_height = std::max<uint32_t>(1u, next_height / 2);
        next_size = next_width * next_height * channels;

        if (next_width == 1 && next_height == 1) {
            break;
        }
    }
}

std::vector<Mipmap> &Image::mipmaps() { return _mipmaps; }

std::vector<uint8_t> &Image::data() { return _data; }

void Image::setData(const uint8_t *raw_data, size_t size) {
    assert(_data.empty() && "Image data already set");
    _data = {raw_data, raw_data + size};
}

void Image::setFormat(const wgpu::TextureFormat f) { _format = f; }

void Image::setWidth(const uint32_t width) { _mipmaps.at(0).extent.width = width; }

void Image::setHeight(const uint32_t height) { _mipmaps.at(0).extent.height = height; }

void Image::setDepth(const uint32_t depth) { _mipmaps.at(0).extent.depthOrArrayLayers = depth; }

void Image::setLayers(uint32_t l) { _mipmaps.at(0).extent.depthOrArrayLayers = l; }

void Image::setOffsets(const std::vector<std::vector<uint64_t>> &o) { _offsets = o; }

std::unique_ptr<Image> Image::load(const std::string &name, const std::string &uri, bool flipY) {
    std::unique_ptr<Image> image{nullptr};

    auto data = fs::ReadAsset(uri);

    // Get extension
    auto extension = fs::ExtraExtension(uri);
    if (extension == "png" || extension == "jpg") {
        image = std::make_unique<Stb>(name, data, flipY);
    } else if (extension == "astc") {
        image = std::make_unique<Astc>(name, data, flipY);
    } else if (extension == "ktx" || extension == "ktx2") {
        image = std::make_unique<Ktx>(name, data, flipY);
    }
    return image;
}

}  // namespace vox
