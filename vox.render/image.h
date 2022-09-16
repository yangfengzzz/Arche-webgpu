//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <webgpu/webgpu_cpp.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace vox {
/**
 * @brief Mipmap information
 */
struct Mipmap {
    /// Mipmap level
    uint32_t level = 0;

    /// Byte offset used for uploading
    uint32_t offset = 0;

    /// Width depth and height of the mipmap
    wgpu::Extent3D extent = {0, 0, 0};
};

class Image {
public:
    std::string name;

    explicit Image(std::string name, std::vector<uint8_t> &&data = {}, std::vector<Mipmap> &&mipmaps = {{}});

    static std::unique_ptr<Image> load(const std::string &name, const std::string &uri, bool flipY = false);

    virtual ~Image() = default;

    [[nodiscard]] const std::vector<uint8_t> &data() const;

    void clear();

    [[nodiscard]] wgpu::TextureFormat format() const;

    [[nodiscard]] const wgpu::Extent3D &extent() const;

    [[nodiscard]] uint32_t layers() const;

    [[nodiscard]] const std::vector<Mipmap> &mipmaps() const;

    [[nodiscard]] const std::vector<std::vector<uint64_t>> &offsets() const;

    void generateMipmaps();

public:
    void createTexture(wgpu::Device &device,
                       wgpu::TextureUsage usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst);

    [[nodiscard]] const wgpu::Texture &GetTexture() const;

    [[nodiscard]] const wgpu::TextureView &getTextureView(wgpu::TextureViewDimension view_type = wgpu::TextureViewDimension::e2D,
                                                        uint32_t base_mip_level = 0,
                                                        uint32_t base_array_layer = 0,
                                                        uint32_t n_mip_levels = 0,
                                                        uint32_t n_array_layers = 0);

protected:
    std::vector<uint8_t> &data();

    void setData(const uint8_t *raw_data, size_t size);

    void setFormat(wgpu::TextureFormat format);

    void setWidth(uint32_t width);

    void setHeight(uint32_t height);

    void setDepth(uint32_t depth);

    void setLayers(uint32_t layers);

    void setOffsets(const std::vector<std::vector<uint64_t>> &offsets);

    Mipmap &mipmap(size_t index);

    std::vector<Mipmap> &mipmaps();

private:
    std::vector<uint8_t> _data;

    wgpu::TextureFormat _format{wgpu::TextureFormat::Undefined};

    uint32_t layers_{1};

    std::vector<Mipmap> _mipmaps{{}};

    // Offsets stored like offsets[array_layer][mipmap_layer]
    std::vector<std::vector<uint64_t>> _offsets;

    wgpu::Texture _texture;

    std::unordered_map<size_t, wgpu::TextureView> _texture_views;
};

}  // namespace vox