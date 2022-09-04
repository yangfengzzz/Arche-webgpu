//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/rendering/resource_cache.h"

#include <utility>

#include "vox.render/std_helpers.h"

namespace std {
using namespace vox;
// MARK: - RenderPipelineDescriptor
template <>
struct hash<wgpu::PrimitiveState> {
    std::size_t operator()(const wgpu::PrimitiveState &state) const {
        std::size_t result = 0;

        hash_combine(result, state.topology);
        hash_combine(result, state.frontFace);
        hash_combine(result, state.cullMode);
        hash_combine(result, state.stripIndexFormat);

        return result;
    }
};

template <>
struct hash<wgpu::MultisampleState> {
    std::size_t operator()(const wgpu::MultisampleState &state) const {
        std::size_t result = 0;

        hash_combine(result, state.count);
        hash_combine(result, state.mask);
        hash_combine(result, state.alphaToCoverageEnabled);

        return result;
    }
};

template <>
struct hash<wgpu::StencilFaceState> {
    std::size_t operator()(const wgpu::StencilFaceState &state) const {
        std::size_t result = 0;

        hash_combine(result, state.compare);
        hash_combine(result, state.depthFailOp);
        hash_combine(result, state.failOp);
        hash_combine(result, state.passOp);

        return result;
    }
};

template <>
struct hash<wgpu::DepthStencilState> {
    std::size_t operator()(const wgpu::DepthStencilState &state) const {
        std::size_t result = 0;

        hash_combine(result, state.format);
        hash_combine(result, state.depthWriteEnabled);
        hash_combine(result, state.depthCompare);
        hash_combine(result, state.stencilBack);
        hash_combine(result, state.stencilFront);
        hash_combine(result, state.stencilReadMask);
        hash_combine(result, state.stencilWriteMask);
        hash_combine(result, state.depthBias);
        hash_combine(result, state.depthBiasClamp);
        hash_combine(result, state.depthBiasSlopeScale);

        return result;
    }
};

template <>
struct hash<wgpu::VertexState> {
    std::size_t operator()(const wgpu::VertexState &state) const {
        std::size_t result = 0;

        hash_combine(result, state.module.Get());  // internal address
        hash_combine(result, state.entryPoint);
        hash_combine(result, state.bufferCount);
        hash_combine(result, state.buffers);  // internal address

        return result;
    }
};

template <>
struct hash<wgpu::FragmentState> {
    std::size_t operator()(const wgpu::FragmentState &state) const {
        std::size_t result = 0;

        hash_combine(result, state.module.Get());  // internal address
        hash_combine(result, state.entryPoint);
        hash_combine(result, state.targetCount);
        for (uint32_t i = 0; i < state.targetCount; i++) {
            const wgpu::ColorTargetState &target = state.targets[i];
            hash_combine(result, target);
        }

        return result;
    }
};

template <>
struct hash<wgpu::ColorTargetState> {
    std::size_t operator()(const wgpu::ColorTargetState &state) const {
        std::size_t result = 0;

        hash_combine(result, state.format);
        hash_combine(result, state.writeMask);
        if (state.blend) {
            hash_combine(result, *state.blend);
        }

        return result;
    }
};

template <>
struct hash<wgpu::BlendState> {
    std::size_t operator()(const wgpu::BlendState &state) const {
        std::size_t result = 0;

        hash_combine(result, state.color);
        hash_combine(result, state.alpha);

        return result;
    }
};

template <>
struct hash<wgpu::BlendComponent> {
    std::size_t operator()(const wgpu::BlendComponent &state) const {
        std::size_t result = 0;

        hash_combine(result, state.operation);
        hash_combine(result, state.srcFactor);
        hash_combine(result, state.dstFactor);

        return result;
    }
};

template <>
struct hash<wgpu::RenderPipelineDescriptor> {
    std::size_t operator()(const wgpu::RenderPipelineDescriptor &descriptor) const {
        std::size_t result = 0;

        hash_combine(result, descriptor.layout.Get());  // internal address
        hash_combine(result, descriptor.primitive);
        hash_combine(result, descriptor.multisample);
        if (descriptor.depthStencil) {
            hash_combine(result, *descriptor.depthStencil);
        }
        hash_combine(result, descriptor.vertex);
        if (descriptor.fragment) {
            hash_combine(result, *descriptor.fragment);
        }

        return result;
    }
};

// MARK: - PipelineLayoutDescriptor
template <>
struct hash<wgpu::PipelineLayoutDescriptor> {
    std::size_t operator()(const wgpu::PipelineLayoutDescriptor &descriptor) const {
        std::size_t result = 0;

        hash_combine(result, descriptor.bindGroupLayoutCount);
        for (uint32_t i = 0; i < descriptor.bindGroupLayoutCount; i++) {
            const wgpu::BindGroupLayout &layout = descriptor.bindGroupLayouts[i];
            hash_combine(result, layout.Get());  // internal address
        }

        return result;
    }
};

// MARK: - BindGroupLayoutDescriptor
template <>
struct hash<wgpu::TextureBindingLayout> {
    std::size_t operator()(const wgpu::TextureBindingLayout &layout) const {
        std::size_t result = 0;

        hash_combine(result, layout.multisampled);
        hash_combine(result, layout.sampleType);
        hash_combine(result, layout.viewDimension);

        return result;
    }
};

template <>
struct hash<wgpu::StorageTextureBindingLayout> {
    std::size_t operator()(const wgpu::StorageTextureBindingLayout &layout) const {
        std::size_t result = 0;

        hash_combine(result, layout.format);
        hash_combine(result, layout.viewDimension);
        hash_combine(result, layout.access);

        return result;
    }
};

template <>
struct hash<wgpu::SamplerBindingLayout> {
    std::size_t operator()(const wgpu::SamplerBindingLayout &layout) const {
        std::size_t result = 0;

        hash_combine(result, layout.type);

        return result;
    }
};

template <>
struct hash<wgpu::BufferBindingLayout> {
    std::size_t operator()(const wgpu::BufferBindingLayout &layout) const {
        std::size_t result = 0;

        hash_combine(result, layout.type);
        hash_combine(result, layout.hasDynamicOffset);
        hash_combine(result, layout.minBindingSize);

        return result;
    }
};

template <>
struct hash<wgpu::BindGroupLayoutEntry> {
    std::size_t operator()(const wgpu::BindGroupLayoutEntry &entry) const {
        std::size_t result = 0;

        hash_combine(result, entry.visibility);
        hash_combine(result, entry.binding);
        hash_combine(result, entry.buffer);
        hash_combine(result, entry.sampler);
        hash_combine(result, entry.texture);
        hash_combine(result, entry.storageTexture);

        return result;
    }
};

template <>
struct hash<wgpu::BindGroupLayoutDescriptor> {
    std::size_t operator()(const wgpu::BindGroupLayoutDescriptor &descriptor) const {
        std::size_t result = 0;

        hash_combine(result, descriptor.entryCount);
        for (uint32_t i = 0; i < descriptor.entryCount; i++) {
            const wgpu::BindGroupLayoutEntry &entry = descriptor.entries[i];
            hash_combine(result, entry);
        }

        return result;
    }
};

// MARK: - BindGroupDescriptor
template <>
struct hash<wgpu::BindGroupEntry> {
    std::size_t operator()(const wgpu::BindGroupEntry &entry) const {
        std::size_t result = 0;

        hash_combine(result, entry.size);
        hash_combine(result, entry.offset);
        hash_combine(result, entry.buffer.Get());  // internal address
        hash_combine(result, entry.binding);
        hash_combine(result, entry.sampler.Get());      // internal address
        hash_combine(result, entry.textureView.Get());  // internal address

        return result;
    }
};

template <>
struct hash<wgpu::BindGroupDescriptor> {
    std::size_t operator()(const wgpu::BindGroupDescriptor &descriptor) const {
        std::size_t result = 0;

        hash_combine(result, descriptor.layout.Get());  // internal address
        hash_combine(result, descriptor.entryCount);
        for (uint32_t i = 0; i < descriptor.entryCount; i++) {
            const wgpu::BindGroupEntry &entry = descriptor.entries[i];
            hash_combine(result, entry);
        }

        return result;
    }
};

// MARK: - ComputePipelineDescriptor
template <>
struct hash<wgpu::ProgrammableStageDescriptor> {
    std::size_t operator()(const wgpu::ProgrammableStageDescriptor &descriptor) const {
        std::size_t result = 0;

        hash_combine(result, descriptor.module.Get());  // internal address
        hash_combine(result, descriptor.entryPoint);

        return result;
    }
};

template <>
struct hash<wgpu::ComputePipelineDescriptor> {
    std::size_t operator()(const wgpu::ComputePipelineDescriptor &descriptor) const {
        std::size_t result = 0;

        hash_combine(result, descriptor.layout.Get());  // internal address
        hash_combine(result, descriptor.compute);

        return result;
    }
};

}  // namespace std

// MARK: - ResourceCache
namespace vox {
ResourceCache::ResourceCache(wgpu::Device &device) : _device{device} {}

wgpu::BindGroupLayout &ResourceCache::requestBindGroupLayout(wgpu::BindGroupLayoutDescriptor &descriptor) {
    std::hash<wgpu::BindGroupLayoutDescriptor> hasher;
    size_t hash = hasher(descriptor);

    auto iter = _state.bindGroupLayouts.find(hash);
    if (iter == _state.bindGroupLayouts.end()) {
        _state.bindGroupLayouts[hash] = _device.CreateBindGroupLayout(&descriptor);
        return _state.bindGroupLayouts[hash];
    } else {
        return iter->second;
    }
}

wgpu::PipelineLayout &ResourceCache::requestPipelineLayout(wgpu::PipelineLayoutDescriptor &descriptor) {
    std::hash<wgpu::PipelineLayoutDescriptor> hasher;
    size_t hash = hasher(descriptor);

    auto iter = _state.pipelineLayouts.find(hash);
    if (iter == _state.pipelineLayouts.end()) {
        _state.pipelineLayouts[hash] = _device.CreatePipelineLayout(&descriptor);
        return _state.pipelineLayouts[hash];
    } else {
        return iter->second;
    }
}

wgpu::BindGroup &ResourceCache::requestBindGroup(wgpu::BindGroupDescriptor &descriptor) {
    std::hash<wgpu::BindGroupDescriptor> hasher;
    size_t hash = hasher(descriptor);

    auto iter = _state.bindGroups.find(hash);
    if (iter == _state.bindGroups.end()) {
        _state.bindGroups[hash] = _device.CreateBindGroup(&descriptor);
        return _state.bindGroups[hash];
    } else {
        return iter->second;
    }
}

wgpu::RenderPipeline &ResourceCache::requestPipeline(wgpu::RenderPipelineDescriptor &descriptor) {
    std::hash<wgpu::RenderPipelineDescriptor> hasher;
    size_t hash = hasher(descriptor);

    auto iter = _state.renderPipelines.find(hash);
    if (iter == _state.renderPipelines.end()) {
        _state.renderPipelines[hash] = _device.CreateRenderPipeline(&descriptor);
        return _state.renderPipelines[hash];
    } else {
        return iter->second;
    }
}

wgpu::ComputePipeline &ResourceCache::requestPipeline(wgpu::ComputePipelineDescriptor &descriptor) {
    std::hash<wgpu::ComputePipelineDescriptor> hasher;
    size_t hash = hasher(descriptor);

    auto iter = _state.computePipelines.find(hash);
    if (iter == _state.computePipelines.end()) {
        _state.computePipelines[hash] = _device.CreateComputePipeline(&descriptor);
        return _state.computePipelines[hash];
    } else {
        return iter->second;
    }
}

ShaderModule &ResourceCache::requestShaderModule(wgpu::ShaderStage stage,
                                                 const ShaderSource &glsl_source,
                                                 const ShaderVariant &shader_variant) {
    std::size_t hash{0U};
    hash_combine(hash, std::hash<std::string>{}(glsl_source.GetSource()));

    auto iter = _state.shaderModules.find(hash);
    if (iter == _state.shaderModules.end()) {
        _state.shaderModules[hash] =
                std::make_unique<ShaderModule>(_device, stage, glsl_source, "main", shader_variant);
        return *_state.shaderModules[hash];
    } else {
        return *iter->second;
    }
}

}  // namespace vox
