//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/shader/shader_data.h"

namespace vox {
ShaderData::ShaderData(wgpu::Device &device) : _device(device) {}

void ShaderData::bindData(
        const std::unordered_map<std::string, ShaderResource> &resources,
        std::unordered_map<uint32_t, std::vector<wgpu::BindGroupLayoutEntry>> &bindGroupLayoutEntryVecMap,
        std::unordered_map<uint32_t, std::vector<wgpu::BindGroupEntry>> &bindGroupEntryVecMap) {
    for (auto &buffer : _shaderBuffers) {
        auto iter = resources.find(buffer.first);
        if (iter != resources.end()) {
            ShaderData::bindBuffer(iter->second, buffer.second, bindGroupLayoutEntryVecMap, bindGroupEntryVecMap);
        }
    }

    for (auto &texture : _shaderTextures) {
        auto iter = resources.find(texture.first);
        if (iter != resources.end()) {
            ShaderData::bindTexture(iter->second, texture.second, bindGroupLayoutEntryVecMap, bindGroupEntryVecMap);
        }
    }

    for (auto &sampler : _shaderSamplers) {
        auto iter = resources.find(sampler.first);
        if (iter != resources.end()) {
            ShaderData::bindSampler(iter->second, sampler.second, bindGroupLayoutEntryVecMap, bindGroupEntryVecMap);
        }
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
        layout_entry.buffer.type = wgpu::BufferBindingType::Uniform;
        bindGroupLayoutEntryVecMap[resource.set].push_back(layout_entry);
    };

    auto bindGroupLayoutIter = bindGroupLayoutEntryVecMap.find(resource.set);
    if (bindGroupLayoutIter != bindGroupLayoutEntryVecMap.end()) {
        bool alreadyExist = false;
        for (auto &bindGroupLayout : bindGroupLayoutIter->second) {
            if (bindGroupLayout.binding == resource.binding) {
                bindGroupLayout.visibility &= resource.stages;
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
        const SampledTexturePtr &texture,
        std::unordered_map<uint32_t, std::vector<wgpu::BindGroupLayoutEntry>> &bindGroupLayoutEntryVecMap,
        std::unordered_map<uint32_t, std::vector<wgpu::BindGroupEntry>> &bindGroupEntryVecMap) {
    auto insertFunctor = [&]() {
        wgpu::BindGroupEntry entry;
        entry.binding = resource.binding;
        entry.textureView = texture->textureView();
        bindGroupEntryVecMap[resource.set].push_back(entry);

        wgpu::BindGroupLayoutEntry layout_entry;
        layout_entry.binding = resource.binding;
        layout_entry.visibility = resource.stages;
        layout_entry.texture.sampleType;
        layout_entry.texture.multisampled = false;
        layout_entry.texture.viewDimension = texture->textureViewDimension();
        bindGroupLayoutEntryVecMap[resource.set].push_back(layout_entry);
    };

    auto bindGroupLayoutIter = bindGroupLayoutEntryVecMap.find(resource.set);
    if (bindGroupLayoutIter != bindGroupLayoutEntryVecMap.end()) {
        bool alreadyExist = false;
        for (auto &bindGroupLayout : bindGroupLayoutIter->second) {
            if (bindGroupLayout.binding == resource.binding) {
                bindGroupLayout.visibility &= resource.stages;
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
        const SampledTexturePtr &sampler,
        std::unordered_map<uint32_t, std::vector<wgpu::BindGroupLayoutEntry>> &bindGroupLayoutEntryVecMap,
        std::unordered_map<uint32_t, std::vector<wgpu::BindGroupEntry>> &bindGroupEntryVecMap) {
    auto insertFunctor = [&]() {
        wgpu::BindGroupEntry entry;
        entry.binding = resource.binding;
        entry.sampler = sampler->sampler();
        bindGroupEntryVecMap[resource.set].push_back(entry);

        wgpu::BindGroupLayoutEntry layout_entry;
        layout_entry.binding = resource.binding;
        layout_entry.visibility = resource.stages;
        layout_entry.sampler.type;
        bindGroupLayoutEntryVecMap[resource.set].push_back(layout_entry);
    };

    auto bindGroupLayoutIter = bindGroupLayoutEntryVecMap.find(resource.set);
    if (bindGroupLayoutIter != bindGroupLayoutEntryVecMap.end()) {
        bool alreadyExist = false;
        for (auto &bindGroupLayout : bindGroupLayoutIter->second) {
            if (bindGroupLayout.binding == resource.binding) {
                bindGroupLayout.visibility &= resource.stages;
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

void ShaderData::setBufferFunctor(const std::string &property, const std::function<Buffer()> &functor) {
    _shaderBufferFunctors.insert(std::make_pair(property, functor));
}

// MARK: - Sampler&&Texture
void ShaderData::setSampledTexture(const std::string &texture_prop,
                                   const std::string &sample_prop,
                                   const SampledTexturePtr &value) {
    _shaderTextures[texture_prop] = value;
    _shaderSamplers[sample_prop] = value;
}

// MARK: - Macro
void ShaderData::addDefine(const std::string &def) { variant_.AddDefine(def); }

void ShaderData::removeDefine(const std::string &undef) { variant_.RemoveDefine(undef); }

void ShaderData::mergeVariants(const ShaderVariant &variant, ShaderVariant &result) const {
    ShaderVariant::UnionCollection(variant, variant_, result);
}

}  // namespace vox
