//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/image_manager.h"

#include "vox.render/shader/shader_manager.h"

namespace vox {
ImageManager *ImageManager::GetSingletonPtr() { return ms_singleton; }

ImageManager &ImageManager::GetSingleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

ImageManager::ImageManager(wgpu::Device &device)
    : _device(device),
      _shaderData(device),
      _bufferPool(device, 1024, wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst) {}

void ImageManager::collectGarbage() {
    for (auto &image : _image_pool) {
        if (image.second.use_count() == 1) {
            image.second.reset();
        }
    }
}

std::shared_ptr<Image> ImageManager::loadTexture(const std::string &file) {
    auto iter = _image_pool.find(file);
    if (iter != _image_pool.end()) {
        return iter->second;
    } else {
        auto image = Image::load(file, file);
        image->createTexture(_device);
        uploadImage(image.get());
        _image_pool.insert(std::make_pair(file, image));
        return image;
    }
}

void ImageManager::uploadImage(Image *image) {
    auto data = image->data();
    auto &mipmaps = image->mipmaps();
    const auto &layers = image->layers();
    auto &offsets = image->offsets();
    auto bytesPerPixel = _bytesPerPixel(image->format());

    for (uint32_t layer = 0; layer < layers; layer++) {
        for (uint32_t i = 0; i < mipmaps.size(); i++) {
            auto width = image->extent().width >> i;
            auto height = image->extent().height >> i;

            wgpu::ImageCopyTexture imageCopyTexture;
            imageCopyTexture.texture = image->getTexture();
            imageCopyTexture.mipLevel = i;
            imageCopyTexture.origin = {0, 0, layer};
            imageCopyTexture.aspect = wgpu::TextureAspect::All;

            wgpu::Extent3D copySize = {width, height, 1};

            wgpu::TextureDataLayout textureDataLayout;
            textureDataLayout.offset = layers > 1 ? offsets[layer][i] : mipmaps[i].offset;
            textureDataLayout.bytesPerRow = bytesPerPixel * width;
            textureDataLayout.rowsPerImage = height;

            _device.GetQueue().WriteTexture(&imageCopyTexture, data.data(), data.size(), &textureDataLayout, &copySize);
        }
    }
}

std::shared_ptr<Image> ImageManager::generateIBL(const std::string &file) {
    auto iter = _image_pool.find(file + "ibl");
    if (iter != _image_pool.end()) {
        return iter->second;
    } else {
        wgpu::CommandEncoder commandEncoder = _device.CreateCommandEncoder();
        auto encoder = commandEncoder.BeginComputePass();
        auto source = loadTexture(file);
        auto baker_mipmap_count = static_cast<uint32_t>(source->mipmaps().size());
        std::vector<Mipmap> mipmap = source->mipmaps();

        auto target = std::make_shared<Image>(file + "ibl", std::vector<uint8_t>(), std::move(mipmap));
        target->setLayers(source->layers());
        target->setFormat(source->format());
        target->createTexture(_device, wgpu::TextureUsage::StorageBinding | wgpu::TextureUsage::TextureBinding);

        if (!_pass) {
            _pass = std::make_unique<ComputePass>(
                    _device, ShaderManager::GetSingleton().LoadShader("ibl.comp"));
            _pass->attachShaderData(&_shaderData);
        }
        _shaderData.setImageView("u_environmentTexture", "u_environmentSampler",
                                 source->getImageView(wgpu::TextureViewDimension::Cube));
        uint32_t source_width = source->extent().width;
        _shaderData.setData("textureSize", source_width);
        for (uint32_t lod = 0; lod < baker_mipmap_count; lod++) {
            float lod_roughness = float(lod) / float(baker_mipmap_count - 1);  // linear
            auto &bufferBlock = _bufferPool.requestBufferBlock(sizeof(float));
            auto allocation = bufferBlock.allocate(sizeof(float));
            allocation.update(_device, lod_roughness);
            _shaderData.setData("lodRoughness", std::move(allocation));

            _shaderData.setStorageImageView("o_results",
                                            target->getImageView(wgpu::TextureViewDimension::e2DArray, lod, 0, 1, 0));
            _pass->setDispatchCount((source_width + 8) / 8, (source_width + 8) / 8, 6);
            _pass->compute(encoder);
        }
        encoder.End();
        wgpu::CommandBuffer commands = commandEncoder.Finish();
        _device.GetQueue().Submit(1, &commands);

        _image_pool.insert(std::make_pair(file + "ibl", target));
        return target;
    }
}

SphericalHarmonics3 ImageManager::generateSH(const std::string &file) {
    auto source = loadTexture(file);
    const auto &layers = source->layers();
    auto &offsets = source->offsets();
    uint32_t texture_size = source->extent().width;
    float texel_size = 2.f / static_cast<float>(texture_size);  // convolution is in the space of [-1, 1]

    float solid_angle_sum = 0;
    const uint32_t kChannelLength = _bytesPerPixel(source->format()) / 2;
    const uint32_t kChannelShift = kChannelLength / 4;
    const float kTotalColor = std::powf(256.f, static_cast<float>(kChannelShift)) - 1;
    SphericalHarmonics3 sh;
    for (uint32_t layer = 0; layer < layers; layer++) {
        uint64_t buffer_offset = offsets[layer][0];
        float v = texel_size * 0.5f - 1.f;
        for (uint32_t y = 0; y < texture_size; y++) {
            float u = texel_size * 0.5f - 1.f;
            for (uint32_t x = 0; x < texture_size; x++) {
                uint64_t data_offset = y * texture_size * kChannelLength + x * kChannelLength + buffer_offset;

                float r = 0.f;
                for (uint8_t i = 0; i < kChannelShift; i++) {
                    r += static_cast<float>(source->_data[data_offset + i]) * std::powf(256.f, i);
                }

                float g = 0.f;
                for (uint8_t i = 0; i < kChannelShift; i++) {
                    g += static_cast<float>(source->_data[data_offset + i + kChannelShift]) * std::powf(256.f, i);
                }

                float b = 0.f;
                for (uint8_t i = 0; i < kChannelShift; i++) {
                    b += static_cast<float>(source->_data[data_offset + i + 2 * kChannelShift]) * std::powf(256.f, i);
                }
                Color color(r / kTotalColor, g / kTotalColor, b / kTotalColor, 0);

                Vector3F direction;
                switch (layer) {
                    case 0:  // TextureCubeFace.PositiveX
                        direction.set(1, -v, -u);
                        break;
                    case 1:  // TextureCubeFace.NegativeX
                        direction.set(-1, -v, u);
                        break;
                    case 2:  // TextureCubeFace.PositiveY
                        direction.set(u, -1, -v);
                        break;
                    case 3:  // TextureCubeFace.NegativeY
                        direction.set(u, 1, v);
                        break;
                    case 4:  // TextureCubeFace.PositiveZ
                        direction.set(u, -v, 1);
                        break;
                    case 5:  // TextureCubeFace.NegativeZ
                        direction.set(-u, -v, -1);
                        break;
                    default:
                        break;
                }

                /**
                 * dA = cos = S / r = 4 / r
                 * dw = dA / r2 = 4 / r / r2
                 */
                float solid_angle = 4.f / (direction.length() * direction.lengthSquared());
                solid_angle_sum += solid_angle;
                sh.addLight(direction.normalized(), color, solid_angle);
                u += texel_size;
            }
            v += texel_size;
        }
    }
    sh = sh * (static_cast<float>(4.0 * M_PI) / solid_angle_sum);
    return sh;
}

wgpu::ImageCopyBuffer ImageManager::_createImageCopyBuffer(wgpu::Buffer buffer,
                                                           uint64_t offset,
                                                           uint32_t bytesPerRow,
                                                           uint32_t rowsPerImage) {
    wgpu::TextureDataLayout textureDataLayout;
    textureDataLayout.offset = offset;
    textureDataLayout.bytesPerRow = bytesPerRow;
    textureDataLayout.rowsPerImage = rowsPerImage;

    wgpu::ImageCopyBuffer imageCopyBuffer = {};
    imageCopyBuffer.buffer = std::move(buffer);
    imageCopyBuffer.layout = textureDataLayout;

    return imageCopyBuffer;
}

uint32_t ImageManager::_bytesPerPixel(wgpu::TextureFormat format) {
    switch (format) {
        case wgpu::TextureFormat::R8Unorm:
        case wgpu::TextureFormat::R8Snorm:
        case wgpu::TextureFormat::R8Uint:
        case wgpu::TextureFormat::R8Sint:
            return 1;

        case wgpu::TextureFormat::R16Uint:
        case wgpu::TextureFormat::R16Sint:
        case wgpu::TextureFormat::R16Float:
        case wgpu::TextureFormat::RG8Unorm:
        case wgpu::TextureFormat::RG8Snorm:
            return 2;

        case wgpu::TextureFormat::R32Uint:
        case wgpu::TextureFormat::R32Sint:
        case wgpu::TextureFormat::R32Float:
        case wgpu::TextureFormat::RG16Uint:
        case wgpu::TextureFormat::RG16Sint:
        case wgpu::TextureFormat::RG16Float:
        case wgpu::TextureFormat::RGBA8Unorm:
        case wgpu::TextureFormat::RGBA8UnormSrgb:
        case wgpu::TextureFormat::RGBA8Snorm:
        case wgpu::TextureFormat::RGBA8Uint:
        case wgpu::TextureFormat::RGBA8Sint:
        case wgpu::TextureFormat::BGRA8Unorm:
        case wgpu::TextureFormat::BGRA8UnormSrgb:
            return 4;

        case wgpu::TextureFormat::RG32Uint:
        case wgpu::TextureFormat::RG32Sint:
        case wgpu::TextureFormat::RG32Float:
        case wgpu::TextureFormat::RGBA16Uint:
        case wgpu::TextureFormat::RGBA16Sint:
        case wgpu::TextureFormat::RGBA16Float:
            return 8;

        case wgpu::TextureFormat::RGBA32Sint:
        case wgpu::TextureFormat::RGBA32Uint:
        case wgpu::TextureFormat::RGBA32Float:
            return 16;

        default:
            assert(false && "undefined");
            throw std::exception();
    }
}

}  // namespace vox
