//
//  image.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/30.
//

#ifndef image_hpp
#define image_hpp

#include <dawn/webgpu_cpp.h>
#include <string>
#include <vector>

namespace vox {
class SampledTexture;

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
    Image(std::vector<uint8_t> &&data = {}, std::vector<Mipmap> &&mipmaps = {{}});
    
    static std::unique_ptr<Image> load(const std::string &name, const std::string &uri);
    
    virtual ~Image() = default;
    
    const std::vector<uint8_t> &data() const;
    
    void clear();
    
    wgpu::TextureFormat format() const;
    
    const wgpu::Extent3D &extent() const;
    
    const uint32_t layers() const;
    
    const std::vector<Mipmap> &mipmaps() const;
    
    const std::vector<std::vector<uint64_t>> &offsets() const;
    
    void generateMipmaps();
    
    std::shared_ptr<SampledTexture> createSampledTexture(wgpu::Device const &device);
    
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
    
    std::vector<Mipmap> _mipmaps{{}};
    
    // Offsets stored like offsets[array_layer][mipmap_layer]
    std::vector<std::vector<uint64_t>> _offsets;
};

}

#endif /* image_hpp */
