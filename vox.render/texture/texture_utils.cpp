//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/texture/texture_utils.h"

#include <array>

namespace vox {
void TextureUtils::buildTextureArray(const std::vector<wgpu::Texture>::iterator& texturesBegin,
                                     const std::vector<wgpu::Texture>::iterator& texturesEnd,
                                     uint32_t width,
                                     uint32_t height,
                                     wgpu::Texture& textureArray,
                                     wgpu::CommandEncoder& commandEncoder) {
    wgpu::ImageCopyTexture destination;
    destination.texture = textureArray;

    wgpu::ImageCopyTexture source;

    wgpu::Extent3D copySize;
    copySize.width = width;
    copySize.height = height;
    copySize.depthOrArrayLayers = 1;

    for (auto iter = texturesBegin; iter < texturesEnd; iter++) {
        destination.origin.z = static_cast<uint32_t>(iter - texturesBegin);
        source.texture = *iter;

        commandEncoder.CopyTextureToTexture(&source, &destination, &copySize);
    }
}

void TextureUtils::buildCubeTextureArray(const std::vector<wgpu::Texture>::iterator& texturesBegin,
                                         const std::vector<wgpu::Texture>::iterator& texturesEnd,
                                         uint32_t width,
                                         uint32_t height,
                                         wgpu::Texture& textureArray,
                                         wgpu::CommandEncoder& commandEncoder) {
    wgpu::ImageCopyTexture destination;
    destination.texture = textureArray;

    wgpu::ImageCopyTexture source;

    wgpu::Extent3D copySize;
    copySize.width = width;
    copySize.height = height;
    copySize.depthOrArrayLayers = 6;

    for (auto iter = texturesBegin; iter < texturesEnd; iter++) {
        destination.origin.z = 6 * static_cast<uint32_t>(iter - texturesBegin);
        source.texture = *iter;

        commandEncoder.CopyTextureToTexture(&source, &destination, &copySize);
    }
}

static wgpu::TextureSampleType sampleType(wgpu::TextureFormat format) {
    switch (format) {
        case wgpu::TextureFormat::Stencil8:
        case wgpu::TextureFormat::Undefined:
            return wgpu::TextureSampleType::Undefined;

        case wgpu::TextureFormat::R8Unorm:
        case wgpu::TextureFormat::R8Uint:
        case wgpu::TextureFormat::R16Uint:
        case wgpu::TextureFormat::RG8Unorm:
        case wgpu::TextureFormat::RG8Uint:
        case wgpu::TextureFormat::R32Uint:
        case wgpu::TextureFormat::RG16Uint:
        case wgpu::TextureFormat::RGBA8Unorm:
        case wgpu::TextureFormat::RGBA8UnormSrgb:
        case wgpu::TextureFormat::RGBA8Uint:
        case wgpu::TextureFormat::BGRA8Unorm:
        case wgpu::TextureFormat::BGRA8UnormSrgb:
        case wgpu::TextureFormat::RGB10A2Unorm:
        case wgpu::TextureFormat::RG32Uint:
        case wgpu::TextureFormat::RGBA16Uint:
        case wgpu::TextureFormat::RGBA32Uint:
        case wgpu::TextureFormat::BC1RGBAUnorm:
        case wgpu::TextureFormat::BC1RGBAUnormSrgb:
        case wgpu::TextureFormat::BC2RGBAUnorm:
        case wgpu::TextureFormat::BC2RGBAUnormSrgb:
        case wgpu::TextureFormat::BC3RGBAUnorm:
        case wgpu::TextureFormat::BC3RGBAUnormSrgb:
        case wgpu::TextureFormat::BC4RUnorm:
        case wgpu::TextureFormat::BC5RGUnorm:
        case wgpu::TextureFormat::BC7RGBAUnorm:
        case wgpu::TextureFormat::BC7RGBAUnormSrgb:
        case wgpu::TextureFormat::ETC2RGB8Unorm:
        case wgpu::TextureFormat::ETC2RGB8UnormSrgb:
        case wgpu::TextureFormat::ETC2RGB8A1Unorm:
        case wgpu::TextureFormat::ETC2RGB8A1UnormSrgb:
        case wgpu::TextureFormat::ETC2RGBA8Unorm:
        case wgpu::TextureFormat::ETC2RGBA8UnormSrgb:
        case wgpu::TextureFormat::EACR11Unorm:
        case wgpu::TextureFormat::EACRG11Unorm:
        case wgpu::TextureFormat::ASTC4x4Unorm:
        case wgpu::TextureFormat::ASTC4x4UnormSrgb:
        case wgpu::TextureFormat::ASTC5x4Unorm:
        case wgpu::TextureFormat::ASTC5x4UnormSrgb:
        case wgpu::TextureFormat::ASTC5x5Unorm:
        case wgpu::TextureFormat::ASTC5x5UnormSrgb:
        case wgpu::TextureFormat::ASTC6x5Unorm:
        case wgpu::TextureFormat::ASTC6x5UnormSrgb:
        case wgpu::TextureFormat::ASTC6x6Unorm:
        case wgpu::TextureFormat::ASTC6x6UnormSrgb:
        case wgpu::TextureFormat::ASTC8x5Unorm:
        case wgpu::TextureFormat::ASTC8x5UnormSrgb:
        case wgpu::TextureFormat::ASTC8x6Unorm:
        case wgpu::TextureFormat::ASTC8x6UnormSrgb:
        case wgpu::TextureFormat::ASTC8x8Unorm:
        case wgpu::TextureFormat::ASTC8x8UnormSrgb:
        case wgpu::TextureFormat::ASTC10x5Unorm:
        case wgpu::TextureFormat::ASTC10x5UnormSrgb:
        case wgpu::TextureFormat::ASTC10x6Unorm:
        case wgpu::TextureFormat::ASTC10x6UnormSrgb:
        case wgpu::TextureFormat::ASTC10x8Unorm:
        case wgpu::TextureFormat::ASTC10x8UnormSrgb:
        case wgpu::TextureFormat::ASTC10x10Unorm:
        case wgpu::TextureFormat::ASTC10x10UnormSrgb:
        case wgpu::TextureFormat::ASTC12x10Unorm:
        case wgpu::TextureFormat::ASTC12x10UnormSrgb:
        case wgpu::TextureFormat::ASTC12x12Unorm:
        case wgpu::TextureFormat::ASTC12x12UnormSrgb:
        case wgpu::TextureFormat::R8BG8Biplanar420Unorm:
            return wgpu::TextureSampleType::Uint;

        case wgpu::TextureFormat::R8Snorm:
        case wgpu::TextureFormat::R8Sint:
        case wgpu::TextureFormat::R16Sint:
        case wgpu::TextureFormat::RG8Snorm:
        case wgpu::TextureFormat::RG8Sint:
        case wgpu::TextureFormat::R32Sint:
        case wgpu::TextureFormat::RG16Sint:
        case wgpu::TextureFormat::RGBA8Snorm:
        case wgpu::TextureFormat::RGBA8Sint:
        case wgpu::TextureFormat::RG32Sint:
        case wgpu::TextureFormat::RGBA16Sint:
        case wgpu::TextureFormat::RGBA32Sint:
        case wgpu::TextureFormat::BC4RSnorm:
        case wgpu::TextureFormat::BC5RGSnorm:
        case wgpu::TextureFormat::EACR11Snorm:
        case wgpu::TextureFormat::EACRG11Snorm:
            return wgpu::TextureSampleType::Sint;

        case wgpu::TextureFormat::R16Float:
        case wgpu::TextureFormat::R32Float:
        case wgpu::TextureFormat::RG16Float:
        case wgpu::TextureFormat::RG11B10Ufloat:
        case wgpu::TextureFormat::RGB9E5Ufloat:
        case wgpu::TextureFormat::RG32Float:
        case wgpu::TextureFormat::RGBA16Float:
        case wgpu::TextureFormat::RGBA32Float:
        case wgpu::TextureFormat::BC6HRGBUfloat:
        case wgpu::TextureFormat::BC6HRGBFloat:
            return wgpu::TextureSampleType::Float;

        case wgpu::TextureFormat::Depth16Unorm:
        case wgpu::TextureFormat::Depth24Plus:
        case wgpu::TextureFormat::Depth24PlusStencil8:
        case wgpu::TextureFormat::Depth32Float:
        case wgpu::TextureFormat::Depth32FloatStencil8:
            return wgpu::TextureSampleType::Depth;
    }
}

}  // namespace vox
