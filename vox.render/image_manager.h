//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/spherical_harmonics3.h"
#include "vox.render/image.h"
#include "vox.render/singleton.h"

namespace vox {
class ImageManager : public Singleton<ImageManager> {
public:
    static ImageManager &GetSingleton();

    static ImageManager *GetSingletonPtr();

    explicit ImageManager(wgpu::Device &device);

    ~ImageManager() = default;

    void collectGarbage();

public:
    /**
     * @brief Loads in a ktx 2D texture
     */
    std::shared_ptr<Image> loadTexture(const std::string &file);

    void uploadImage(Image *image);

public:
    std::shared_ptr<Image> generateIBL(const std::string &file);

    SphericalHarmonics3 generateSH(const std::string &file);

private:
    static uint32_t _bytesPerPixel(wgpu::TextureFormat format);

    static wgpu::ImageCopyBuffer _createImageCopyBuffer(wgpu::Buffer buffer,
                                                        uint64_t offset,
                                                        uint32_t bytesPerRow,
                                                        uint32_t rowsPerImage = wgpu::kCopyStrideUndefined);

    wgpu::Device &_device;
    std::unordered_map<std::string, std::shared_ptr<Image>> _image_pool{};
};
template <>
inline ImageManager *Singleton<ImageManager>::ms_singleton{nullptr};

}  // namespace vox