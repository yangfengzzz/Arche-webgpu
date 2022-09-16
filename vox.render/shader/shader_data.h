//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <webgpu/webgpu_cpp.h>

#include <unordered_map>

#include "vox.render/image_view.h"
#include "vox.render/mesh/buffer.h"
#include "vox.render/shader/shader_module.h"
#include "vox.render/shader/shader_variant.h"
#include "vox.render/std_helpers.h"

namespace vox {
/**
 * Shader data collection,Correspondence includes shader properties data and macros data.
 */
class ShaderData {
public:
    explicit ShaderData(wgpu::Device& device);

    void bindData(const std::unordered_map<std::string, ShaderResource>& resources,
                  std::unordered_map<uint32_t, std::vector<wgpu::BindGroupLayoutEntry>>& bindGroupLayoutEntryVecMap,
                  std::unordered_map<uint32_t, std::vector<wgpu::BindGroupEntry>>& bindGroupEntryVecMap);

    void setBufferFunctor(const std::string& property_name, const std::function<Buffer()>& functor);

    template <typename T>
    void setData(const std::string& property, const T& value) {
        auto iter = _shaderBuffers.find(property);
        if (iter == _shaderBuffers.end()) {
            _shaderBuffers.insert(std::make_pair(
                    property, Buffer(_device, sizeof(T), wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst)));
        }
        iter = _shaderBuffers.find(property);

        _device.GetQueue().WriteBuffer(iter->second.handle(), 0, &value, sizeof(T));
    }

    template <typename T>
    void setData(const std::string& property, const std::vector<T>& value) {
        auto iter = _shaderBuffers.find(property);
        if (iter == _shaderBuffers.end()) {
            _shaderBuffers.insert(
                    std::make_pair(property, Buffer(_device, sizeof(T) * value.size(),
                                                    wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst)));
        }
        iter = _shaderBuffers.find(property);

        _device.GetQueue().WriteBuffer(iter->second.handle(), 0, value.data(), sizeof(T) * value.size());
    }

    template <typename T, size_t N>
    void setData(const std::string& property, const std::array<T, N>& value) {
        auto iter = _shaderBuffers.find(property);
        if (iter == _shaderBuffers.end()) {
            _shaderBuffers.insert(std::make_pair(
                    property, Buffer(_device, sizeof(T) * N, wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst)));
        }
        iter = _shaderBuffers.find(property);

        _device.GetQueue().WriteBuffer(iter->second.handle(), 0, value.data(), sizeof(T) * N);
    }

public:
    void setImageView(const std::string& texture_name,
                      const std::string& sampler_name,
                      const std::shared_ptr<ImageView>& value);

    void setStorageImageView(const std::string& texture_name, const std::shared_ptr<ImageView>& value);

    void setSampler(const std::string& sampler_name, wgpu::SamplerDescriptor desc);

public:
    /**
     * @brief Adds a define macro to the shader
     * @param def String which should go to the right of a define directive
     */
    void addDefine(const std::string& def);

    /**
     * @brief Adds an undef macro to the shader
     * @param undef String which should go to the right of an undef directive
     */
    void removeDefine(const std::string& undef);

    void mergeVariants(const ShaderVariant& variant, ShaderVariant& result) const;

private:
    static void bindBuffer(
            const ShaderResource& resource,
            const Buffer& buffer,
            std::unordered_map<uint32_t, std::vector<wgpu::BindGroupLayoutEntry>>& bindGroupLayoutEntryVecMap,
            std::unordered_map<uint32_t, std::vector<wgpu::BindGroupEntry>>& bindGroupEntryVecMap,
            bool isUniform);

    static void bindTexture(
            const ShaderResource& resource,
            const std::shared_ptr<ImageView>& imageView,
            std::unordered_map<uint32_t, std::vector<wgpu::BindGroupLayoutEntry>>& bindGroupLayoutEntryVecMap,
            std::unordered_map<uint32_t, std::vector<wgpu::BindGroupEntry>>& bindGroupEntryVecMap);

    static void bindSampler(
            const ShaderResource& resource,
            const wgpu::SamplerDescriptor& sampler,
            std::unordered_map<uint32_t, std::vector<wgpu::BindGroupLayoutEntry>>& bindGroupLayoutEntryVecMap,
            std::unordered_map<uint32_t, std::vector<wgpu::BindGroupEntry>>& bindGroupEntryVecMap);

    wgpu::Device& _device;
    std::unordered_map<std::string, std::function<Buffer()>> _shaderBufferFunctors{};
    std::unordered_map<std::string, Buffer> _shaderBuffers{};
    std::unordered_map<std::string, std::shared_ptr<ImageView>> _imageViews{};
    std::unordered_map<std::string, wgpu::SamplerDescriptor> _samplers{};
    static wgpu::SamplerDescriptor _defaultSamplerDesc;

    ShaderVariant variant_;
};

}  // namespace vox