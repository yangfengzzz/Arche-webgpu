//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/shader/shader_data.h"

#include "vox.render/rendering/resource_cache.h"

namespace vox {
wgpu::SamplerDescriptor ShaderData::_defaultSamplerDesc{};

ShaderData::ShaderData(wgpu::Device &device) : _device(device) {}

void ShaderData::clear() {
    _shaderBufferPools.clear();
    _shaderBufferFunctors.clear();
    _shaderBuffers.clear();
    _imageViews.clear();
    _samplers.clear();
}

void ShaderData::bindData(
        const std::unordered_map<std::string, ShaderResource> &resources,
        std::unordered_map<uint32_t, std::vector<wgpu::BindGroupLayoutEntry>> &bindGroupLayoutEntryVecMap,
        std::unordered_map<uint32_t, std::vector<wgpu::BindGroupEntry>> &bindGroupEntryVecMap) {
    for (auto &buffer : _shaderBufferPools) {
        auto iter = resources.find(buffer.first);
        if (iter != resources.end()) {
            ShaderData::bindBuffer(iter->second, buffer.second, bindGroupLayoutEntryVecMap, bindGroupEntryVecMap);
        }
    }

    for (auto &buffer : _shaderBuffers) {
        auto iter = resources.find(buffer.first);
        if (iter != resources.end()) {
            ShaderData::bindBuffer(iter->second, buffer.second, bindGroupLayoutEntryVecMap, bindGroupEntryVecMap);
        }
    }

    for (auto &bufferFunctor : _shaderBufferFunctors) {
        auto iter = resources.find(bufferFunctor.first);
        if (iter != resources.end()) {
            ShaderData::bindBuffer(iter->second, bufferFunctor.second(), bindGroupLayoutEntryVecMap,
                                   bindGroupEntryVecMap);
        }
    }

    for (auto &texture : _imageViews) {
        auto iter = resources.find(texture.first);
        if (iter != resources.end()) {
            ShaderData::bindTexture(iter->second, texture.second, bindGroupLayoutEntryVecMap, bindGroupEntryVecMap);
        }
    }

    for (auto &sampler : _samplers) {
        auto iter = resources.find(sampler.first);
        if (iter != resources.end()) {
            ShaderData::bindSampler(iter->second, sampler.second, bindGroupLayoutEntryVecMap, bindGroupEntryVecMap);
        }
    }
}

void ShaderData::bindBuffer(
        const ShaderResource &resource,
        const BufferAllocation &bufferAllocation,
        std::unordered_map<uint32_t, std::vector<wgpu::BindGroupLayoutEntry>> &bindGroupLayoutEntryVecMap,
        std::unordered_map<uint32_t, std::vector<wgpu::BindGroupEntry>> &bindGroupEntryVecMap) {
    auto insertFunctor = [&]() {
        wgpu::BindGroupEntry entry;
        entry.binding = resource.binding;
        entry.buffer = bufferAllocation.getBuffer().handle();
        entry.size = bufferAllocation.getSize();
        entry.offset = bufferAllocation.getOffset();
        bindGroupEntryVecMap[resource.set].push_back(entry);

        wgpu::BindGroupLayoutEntry layout_entry;
        layout_entry.binding = resource.binding;
        layout_entry.visibility = resource.stages;
        layout_entry.buffer.type = wgpu::BufferBindingType::Uniform;
        bindGroupLayoutEntryVecMap[resource.set].push_back(layout_entry);
    };

    auto bindGroupLayoutIter = bindGroupLayoutEntryVecMap.find(resource.set);
    if (bindGroupLayoutIter != bindGroupLayoutEntryVecMap.end()) {
        bool alreadyExist = false;
        for (auto &bindGroupLayout : bindGroupLayoutIter->second) {
            if (bindGroupLayout.binding == resource.binding) {
                bindGroupLayout.visibility |= resource.stages;
                alreadyExist = true;
                break;
            }
        }
        if (!alreadyExist) {
            insertFunctor();
        }
    } else {
        bindGroupLayoutEntryVecMap[resource.set] = {};
        bindGroupEntryVecMap[resource.set] = {};
        insertFunctor();
    }
}

void ShaderData::bindBuffer(
        const ShaderResource &resource,
        const Buffer &buffer,
        std::unordered_map<uint32_t, std::vector<wgpu::BindGroupLayoutEntry>> &bindGroupLayoutEntryVecMap,
        std::unordered_map<uint32_t, std::vector<wgpu::BindGroupEntry>> &bindGroupEntryVecMap) {
    auto insertFunctor = [&]() {
        wgpu::BindGroupEntry entry;
        entry.binding = resource.binding;
        entry.buffer = buffer.handle();
        entry.size = buffer.size();
        bindGroupEntryVecMap[resource.set].push_back(entry);

        wgpu::BindGroupLayoutEntry layout_entry;
        layout_entry.binding = resource.binding;
        layout_entry.visibility = resource.stages;
        if (resource.type == ShaderResourceType::BUFFER_STORAGE) {
            layout_entry.buffer.type = wgpu::BufferBindingType::Storage;
        } else {
            layout_entry.buffer.type = wgpu::BufferBindingType::Uniform;
        }
        bindGroupLayoutEntryVecMap[resource.set].push_back(layout_entry);
    };

    auto bindGroupLayoutIter = bindGroupLayoutEntryVecMap.find(resource.set);
    if (bindGroupLayoutIter != bindGroupLayoutEntryVecMap.end()) {
        bool alreadyExist = false;
        for (auto &bindGroupLayout : bindGroupLayoutIter->second) {
            if (bindGroupLayout.binding == resource.binding) {
                bindGroupLayout.visibility |= resource.stages;
                alreadyExist = true;
                break;
            }
        }
        if (!alreadyExist) {
            insertFunctor();
        }
    } else {
        bindGroupLayoutEntryVecMap[resource.set] = {};
        bindGroupEntryVecMap[resource.set] = {};
        insertFunctor();
    }
}

void ShaderData::bindTexture(
        const ShaderResource &resource,
        const std::shared_ptr<ImageView> &imageView,
        std::unordered_map<uint32_t, std::vector<wgpu::BindGroupLayoutEntry>> &bindGroupLayoutEntryVecMap,
        std::unordered_map<uint32_t, std::vector<wgpu::BindGroupEntry>> &bindGroupEntryVecMap) {
    auto insertFunctor = [&]() {
        wgpu::BindGroupEntry entry{};
        entry.binding = resource.binding;
        entry.textureView = imageView->handle();
        bindGroupEntryVecMap[resource.set].push_back(entry);

        wgpu::BindGroupLayoutEntry layout_entry{};
        layout_entry.binding = resource.binding;
        layout_entry.visibility = resource.stages;
        if (resource.type == ShaderResourceType::IMAGE_STORAGE) {
            layout_entry.storageTexture.format = imageView->format();
            layout_entry.storageTexture.access = wgpu::StorageTextureAccess::WriteOnly;
            layout_entry.storageTexture.viewDimension = imageView->dimension();
        } else {
            switch (imageView->format()) {
                case wgpu::TextureFormat::Depth16Unorm:
                case wgpu::TextureFormat::Depth32Float:
                case wgpu::TextureFormat::Depth24Plus:
                case wgpu::TextureFormat::Depth24PlusStencil8:
                case wgpu::TextureFormat::Depth32FloatStencil8:
                    layout_entry.texture.sampleType = wgpu::TextureSampleType::Depth;
                    break;
                default:
                    layout_entry.texture.sampleType = wgpu::TextureSampleType::Float;
            }
            layout_entry.texture.multisampled = imageView->sampleCount() > 1;
            layout_entry.texture.viewDimension = imageView->dimension();
        }
        bindGroupLayoutEntryVecMap[resource.set].push_back(layout_entry);
    };

    auto bindGroupLayoutIter = bindGroupLayoutEntryVecMap.find(resource.set);
    if (bindGroupLayoutIter != bindGroupLayoutEntryVecMap.end()) {
        bool alreadyExist = false;
        for (auto &bindGroupLayout : bindGroupLayoutIter->second) {
            if (bindGroupLayout.binding == resource.binding) {
                bindGroupLayout.visibility |= resource.stages;
                alreadyExist = true;
                break;
            }
        }
        if (!alreadyExist) {
            insertFunctor();
        }
    } else {
        bindGroupLayoutEntryVecMap[resource.set] = {};
        bindGroupEntryVecMap[resource.set] = {};
        insertFunctor();
    }
}

void ShaderData::bindSampler(
        const ShaderResource &resource,
        const wgpu::SamplerDescriptor &sampler,
        std::unordered_map<uint32_t, std::vector<wgpu::BindGroupLayoutEntry>> &bindGroupLayoutEntryVecMap,
        std::unordered_map<uint32_t, std::vector<wgpu::BindGroupEntry>> &bindGroupEntryVecMap) {
    auto insertFunctor = [&]() {
        wgpu::BindGroupEntry entry;
        entry.binding = resource.binding;
        entry.sampler = ResourceCache::GetSingleton().requestSampler(sampler);
        bindGroupEntryVecMap[resource.set].push_back(entry);

        wgpu::BindGroupLayoutEntry layout_entry;
        layout_entry.binding = resource.binding;
        layout_entry.visibility = resource.stages;
        layout_entry.sampler.type = sampler.compare != wgpu::CompareFunction::Undefined
                                            ? wgpu::SamplerBindingType::Comparison
                                            : wgpu::SamplerBindingType::Filtering;
        bindGroupLayoutEntryVecMap[resource.set].push_back(layout_entry);
    };

    auto bindGroupLayoutIter = bindGroupLayoutEntryVecMap.find(resource.set);
    if (bindGroupLayoutIter != bindGroupLayoutEntryVecMap.end()) {
        bool alreadyExist = false;
        for (auto &bindGroupLayout : bindGroupLayoutIter->second) {
            if (bindGroupLayout.binding == resource.binding) {
                bindGroupLayout.visibility |= resource.stages;
                alreadyExist = true;
                break;
            }
        }
        if (!alreadyExist) {
            insertFunctor();
        }
    } else {
        bindGroupLayoutEntryVecMap[resource.set] = {};
        bindGroupEntryVecMap[resource.set] = {};
        insertFunctor();
    }
}

void ShaderData::setData(const std::string &property_name, BufferAllocation &&value) {
    _shaderBufferPools[property_name] = std::move(value);
}

void ShaderData::setBufferFunctor(const std::string &property, const std::function<Buffer()> &functor) {
    _shaderBufferFunctors.insert(std::make_pair(property, functor));
}

// MARK: - Sampler&&Texture
void ShaderData::setImageView(const std::string &texture_name,
                              const std::string &sampler_name,
                              const std::shared_ptr<ImageView> &value) {
    _imageViews[texture_name] = value;
    auto iter = _samplers.find(sampler_name);
    if (iter == _samplers.end()) {
        _samplers[sampler_name] = ShaderData::_defaultSamplerDesc;
    }
}

void ShaderData::setStorageImageView(const std::string &texture_name, const std::shared_ptr<ImageView> &value) {
    _imageViews[texture_name] = value;
}

void ShaderData::setSampler(const std::string &sampler_name, wgpu::SamplerDescriptor value) {
    _samplers[sampler_name] = value;
}

// MARK: - Macro
void ShaderData::addDefine(const std::string &def) { variant_.AddDefine(def); }

void ShaderData::removeDefine(const std::string &undef) { variant_.RemoveDefine(undef); }

void ShaderData::mergeVariants(const ShaderVariant &variant, ShaderVariant &result) const {
    ShaderVariant::UnionCollection(variant, variant_, result);
}

}  // namespace vox
