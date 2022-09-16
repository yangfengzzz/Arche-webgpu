//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/image/ktx_img.h"

#include <ktx.h>
#include <vulkan/vulkan.h>
#include <ktxvulkan.h>

namespace vox {
wgpu::TextureFormat toTextureFormat(VkFormat format) {
    switch (format) {
        case VK_FORMAT_R8_UNORM:
            return wgpu::TextureFormat::R8Unorm;
        case VK_FORMAT_R8_SNORM:
            return wgpu::TextureFormat::R8Snorm;
        case VK_FORMAT_R8_UINT:
            return wgpu::TextureFormat::R8Uint;
        case VK_FORMAT_R8_SINT:
            return wgpu::TextureFormat::R8Sint;
        case VK_FORMAT_R16_UINT:
            return wgpu::TextureFormat::R16Uint;
        case VK_FORMAT_R16_SINT:
            return wgpu::TextureFormat::R16Sint;
        case VK_FORMAT_R16_SFLOAT:
            return wgpu::TextureFormat::R16Float;
        case VK_FORMAT_R8G8_UNORM:
            return wgpu::TextureFormat::RG8Unorm;
        case VK_FORMAT_R8G8_SNORM:
            return wgpu::TextureFormat::RG8Snorm;
        case VK_FORMAT_R8G8_UINT:
            return wgpu::TextureFormat::RG8Uint;
        case VK_FORMAT_R8G8_SINT:
            return wgpu::TextureFormat::RG8Sint;
        case VK_FORMAT_R32_SFLOAT:
            return wgpu::TextureFormat::R32Float;
        case VK_FORMAT_R32_UINT:
            return wgpu::TextureFormat::R32Uint;
        case VK_FORMAT_R32_SINT:
            return wgpu::TextureFormat::R32Sint;
        case VK_FORMAT_R16G16_UINT:
            return wgpu::TextureFormat::RG16Uint;
        case VK_FORMAT_R16G16_SINT:
            return wgpu::TextureFormat::RG16Sint;
        case VK_FORMAT_R16G16_SFLOAT:
            return wgpu::TextureFormat::RG16Float;
        case VK_FORMAT_R8G8B8A8_UNORM:
            return wgpu::TextureFormat::RGBA8Unorm;
        case VK_FORMAT_R8G8B8A8_USCALED:
            return wgpu::TextureFormat::RGBA8UnormSrgb;
        case VK_FORMAT_R8G8B8A8_SNORM:
            return wgpu::TextureFormat::RGBA8Snorm;
        case VK_FORMAT_R8G8B8A8_UINT:
            return wgpu::TextureFormat::RGBA8Uint;
        case VK_FORMAT_R8G8B8A8_SINT:
            return wgpu::TextureFormat::RGBA8Sint;
        case VK_FORMAT_B8G8R8A8_UNORM:
            return wgpu::TextureFormat::BGRA8Unorm;
        case VK_FORMAT_B8G8R8A8_USCALED:
            return wgpu::TextureFormat::BGRA8UnormSrgb;
        case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
            return wgpu::TextureFormat::RGB10A2Unorm;
        case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
            return wgpu::TextureFormat::RGB9E5Ufloat;
        case VK_FORMAT_R32G32_SFLOAT:
            return wgpu::TextureFormat::RG32Float;
        case VK_FORMAT_R32G32_UINT:
            return wgpu::TextureFormat::RG32Uint;
        case VK_FORMAT_R32G32_SINT:
            return wgpu::TextureFormat::RG32Sint;
        case VK_FORMAT_R16G16B16A16_UINT:
            return wgpu::TextureFormat::RGBA16Uint;
        case VK_FORMAT_R16G16B16A16_SINT:
            return wgpu::TextureFormat::RGBA16Sint;
        case VK_FORMAT_R16G16B16A16_SFLOAT:
            return wgpu::TextureFormat::RGBA16Float;
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            return wgpu::TextureFormat::RGBA32Float;
        case VK_FORMAT_R32G32B32A32_UINT:
            return wgpu::TextureFormat::RGBA32Uint;
        case VK_FORMAT_R32G32B32A32_SINT:
            return wgpu::TextureFormat::RGBA32Sint;
        case VK_FORMAT_S8_UINT:
            return wgpu::TextureFormat::Stencil8;
        case VK_FORMAT_D16_UNORM:
            return wgpu::TextureFormat::Depth16Unorm;
        case VK_FORMAT_X8_D24_UNORM_PACK32:
            return wgpu::TextureFormat::Depth24Plus;
        case VK_FORMAT_D24_UNORM_S8_UINT:
            return wgpu::TextureFormat::Depth24PlusStencil8;
        case VK_FORMAT_D32_SFLOAT:
            return wgpu::TextureFormat::Depth32Float;
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return wgpu::TextureFormat::Depth32FloatStencil8;
        case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
            return wgpu::TextureFormat::BC1RGBAUnorm;
        case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
            return wgpu::TextureFormat::BC1RGBAUnormSrgb;
        case VK_FORMAT_BC2_UNORM_BLOCK:
            return wgpu::TextureFormat::BC2RGBAUnorm;
        case VK_FORMAT_BC2_SRGB_BLOCK:
            return wgpu::TextureFormat::BC2RGBAUnormSrgb;
        case VK_FORMAT_BC3_UNORM_BLOCK:
            return wgpu::TextureFormat::BC3RGBAUnorm;
        case VK_FORMAT_BC3_SRGB_BLOCK:
            return wgpu::TextureFormat::BC3RGBAUnormSrgb;
        case VK_FORMAT_BC4_UNORM_BLOCK:
            return wgpu::TextureFormat::BC4RUnorm;
        case VK_FORMAT_BC4_SNORM_BLOCK:
            return wgpu::TextureFormat::BC4RSnorm;
        case VK_FORMAT_BC5_UNORM_BLOCK:
            return wgpu::TextureFormat::BC5RGUnorm;
        case VK_FORMAT_BC5_SNORM_BLOCK:
            return wgpu::TextureFormat::BC5RGSnorm;
        case VK_FORMAT_BC6H_UFLOAT_BLOCK:
            return wgpu::TextureFormat::BC6HRGBUfloat;
        case VK_FORMAT_BC6H_SFLOAT_BLOCK:
            return wgpu::TextureFormat::BC6HRGBFloat;
        case VK_FORMAT_BC7_UNORM_BLOCK:
            return wgpu::TextureFormat::BC7RGBAUnorm;
        case VK_FORMAT_BC7_SRGB_BLOCK:
            return wgpu::TextureFormat::BC7RGBAUnormSrgb;
        case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
            return wgpu::TextureFormat::ETC2RGB8Unorm;
        case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
            return wgpu::TextureFormat::ETC2RGB8UnormSrgb;
        case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
            return wgpu::TextureFormat::ETC2RGB8A1Unorm;
        case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
            return wgpu::TextureFormat::ETC2RGB8A1UnormSrgb;
        case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
            return wgpu::TextureFormat::ETC2RGBA8Unorm;
        case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
            return wgpu::TextureFormat::ETC2RGBA8UnormSrgb;
        case VK_FORMAT_EAC_R11_UNORM_BLOCK:
            return wgpu::TextureFormat::EACR11Unorm;
        case VK_FORMAT_EAC_R11_SNORM_BLOCK:
            return wgpu::TextureFormat::EACR11Snorm;
        case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:
            return wgpu::TextureFormat::EACRG11Unorm;
        case VK_FORMAT_EAC_R11G11_SNORM_BLOCK:
            return wgpu::TextureFormat::EACRG11Snorm;
        case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
            return wgpu::TextureFormat::ASTC4x4Unorm;
        case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
            return wgpu::TextureFormat::ASTC4x4UnormSrgb;
        case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
            return wgpu::TextureFormat::ASTC5x4Unorm;
        case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
            return wgpu::TextureFormat::ASTC5x4UnormSrgb;
        case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
            return wgpu::TextureFormat::ASTC5x5Unorm;
        case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
            return wgpu::TextureFormat::ASTC5x5UnormSrgb;
        case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
            return wgpu::TextureFormat::ASTC6x5Unorm;
        case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
            return wgpu::TextureFormat::ASTC6x5UnormSrgb;
        case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
            return wgpu::TextureFormat::ASTC6x6Unorm;
        case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
            return wgpu::TextureFormat::ASTC6x6UnormSrgb;
        case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
            return wgpu::TextureFormat::ASTC8x5Unorm;
        case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
            return wgpu::TextureFormat::ASTC8x5UnormSrgb;
        case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
            return wgpu::TextureFormat::ASTC8x6Unorm;
        case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
            return wgpu::TextureFormat::ASTC8x6UnormSrgb;
        case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
            return wgpu::TextureFormat::ASTC8x8Unorm;
        case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
            return wgpu::TextureFormat::ASTC8x8UnormSrgb;
        case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
            return wgpu::TextureFormat::ASTC10x5Unorm;
        case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
            return wgpu::TextureFormat::ASTC10x5UnormSrgb;
        case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
            return wgpu::TextureFormat::ASTC10x6Unorm;
        case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
            return wgpu::TextureFormat::ASTC10x6UnormSrgb;
        case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
            return wgpu::TextureFormat::ASTC10x8Unorm;
        case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
            return wgpu::TextureFormat::ASTC10x8UnormSrgb;
        case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
            return wgpu::TextureFormat::ASTC10x10Unorm;
        case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:
            return wgpu::TextureFormat::ASTC10x10UnormSrgb;
        case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
            return wgpu::TextureFormat::ASTC12x10Unorm;
        case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
            return wgpu::TextureFormat::ASTC12x10UnormSrgb;
        case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
            return wgpu::TextureFormat::ASTC12x12Unorm;
        case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:
            return wgpu::TextureFormat::ASTC12x12UnormSrgb;
        case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM:
            return wgpu::TextureFormat::R8BG8Biplanar420Unorm;
        default:
            return wgpu::TextureFormat::Undefined;
    }
}

/// Row padding is different between KTX (pad to 4) and Vulkan (none).
/// Also region->bufferOffset, i.e. the start of each image, has
/// to be a multiple of 4 and also a multiple of the element size.
static ktx_error_code_e KTX_APIENTRY optimal_tiling_callback(int mip_level,
                                                             int face,
                                                             int width,
                                                             int height,
                                                             int depth,
                                                             ktx_uint64_t face_lod_size,
                                                             void *pixels,
                                                             void *user_data) {
    // Get mipmaps
    auto &mipmaps = *reinterpret_cast<std::vector<Mipmap> *>(user_data);
    assert(static_cast<size_t>(mip_level) < mipmaps.size() && "Not enough space in the mipmap vector");

    auto &mipmap = mipmaps.at(mip_level);
    mipmap.level = mip_level;
    mipmap.extent.width = width;
    mipmap.extent.height = height;
    mipmap.extent.depthOrArrayLayers = depth;

    // Set offset for the next mip level
    auto next_mip_level = static_cast<size_t>(mip_level + 1);
    if (next_mip_level < mipmaps.size()) {
        mipmaps.at(next_mip_level).offset = mipmap.offset + static_cast<uint32_t>(face_lod_size);
    }

    return KTX_SUCCESS;
}

Ktx::Ktx(const std::string &name, const std::vector<uint8_t> &data, bool flipY) : Image{name} {
    auto data_buffer = reinterpret_cast<const ktx_uint8_t *>(data.data());
    auto data_size = static_cast<ktx_size_t>(data.size());

    ktxTexture *texture;
    auto load_ktx_result = ktxTexture_CreateFromMemory(data_buffer, data_size, KTX_TEXTURE_CREATE_NO_FLAGS, &texture);
    if (load_ktx_result != KTX_SUCCESS) {
        throw std::runtime_error{"Error loading KTX image"};
    }

    if (texture->pData) {
        // Already loaded
        setData(texture->pData, texture->dataSize);
    } else {
        // Load
        std::vector<uint8_t> &mut_data = this->data();
        auto size = texture->dataSize;
        mut_data.resize(size);
        auto load_data_result = ktxTexture_LoadImageData(texture, mut_data.data(), size);
        if (load_data_result != KTX_SUCCESS) {
            throw std::runtime_error{"Error loading KTX image data"};
        }
    }

    // Update width and height
    setWidth(texture->baseWidth);
    setHeight(texture->baseHeight);
    setDepth(texture->baseDepth);
    setLayers(texture->numLayers);

    bool cubemap = false;

    // Use the faces if there are 6 (for cubemap)
    if (texture->numLayers == 1 && texture->numFaces == 6) {
        cubemap = true;
        setLayers(texture->numFaces);
    }

    // Update format
    auto updated_format = ktxTexture_GetVkFormat(texture);
    setFormat(toTextureFormat(updated_format));

    // Update mip levels
    auto &mipmap_levels = mipmaps();
    mipmap_levels.resize(texture->numLevels);
    auto result = ktxTexture_IterateLevels(texture, optimal_tiling_callback, &mipmap_levels);
    if (result != KTX_SUCCESS) {
        throw std::runtime_error("Error loading KTX image");
    }

    // If the image contains more than one layer, then populate the offsets otherwise take the mipmap level offsets
    if (texture->numLayers > 1 || cubemap) {
        uint32_t layer_count = cubemap ? texture->numFaces : texture->numLayers;

        std::vector<std::vector<uint64_t>> offsets;
        for (uint32_t layer = 0; layer < layer_count; layer++) {
            std::vector<uint64_t> layer_offsets{};
            for (uint32_t level = 0; level < texture->numLevels; level++) {
                ktx_size_t offset;
                KTX_error_code result;
                if (cubemap) {
                    result = ktxTexture_GetImageOffset(texture, level, 0, layer, &offset);
                } else {
                    result = ktxTexture_GetImageOffset(texture, level, layer, 0, &offset);
                }
                layer_offsets.push_back(static_cast<uint64_t>(offset));
            }
            offsets.push_back(layer_offsets);
        }
        setOffsets(offsets);
    } else {
        std::vector<std::vector<uint64_t>> offsets{};
        offsets.resize(1);
        for (auto &mipmap_level : mipmap_levels) {
            offsets[0].push_back(static_cast<uint64_t>(mipmap_level.offset));
        }
        setOffsets(offsets);
    }

    ktxTexture_Destroy(texture);
}

}  // namespace vox
