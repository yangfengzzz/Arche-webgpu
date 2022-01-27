//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "resource_cache.h"
#include "std_helpers.h"
#include <utility>

namespace std {
using namespace vox;
template<>
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

template<>
struct hash<wgpu::MultisampleState> {
    std::size_t operator()(const wgpu::MultisampleState &state) const {
        std::size_t result = 0;
        
        hash_combine(result, state.count);
        hash_combine(result, state.mask);
        hash_combine(result, state.alphaToCoverageEnabled);

        return result;
    }
};

template<>
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

template<>
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

template<>
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

template<>
struct hash<wgpu::FragmentState> {
    std::size_t operator()(const wgpu::FragmentState &state) const {
        std::size_t result = 0;
        
        hash_combine(result, state.module.Get());  // internal address
        hash_combine(result, state.entryPoint);
        hash_combine(result, state.targetCount);
        for (uint32_t i = 0; i < state.targetCount; i++) {
            const wgpu::ColorTargetState& target = state.targets[i];
            hash_combine(result, target);
        }

        return result;
    }
};

template<>
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

template<>
struct hash<wgpu::BlendState> {
    std::size_t operator()(const wgpu::BlendState &state) const {
        std::size_t result = 0;
        
        hash_combine(result, state.color);
        hash_combine(result, state.alpha);

        return result;
    }
};

template<>
struct hash<wgpu::BlendComponent> {
    std::size_t operator()(const wgpu::BlendComponent &state) const {
        std::size_t result = 0;
        
        hash_combine(result, state.operation);
        hash_combine(result, state.srcFactor);
        hash_combine(result, state.dstFactor);

        return result;
    }
};


template<>
struct hash<wgpu::RenderPipelineDescriptor> {
    std::size_t operator()(const wgpu::RenderPipelineDescriptor &descriptor) const {
        std::size_t result = 0;
        
//        hash_combine(result, descriptor.layout); // internal address
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

}

//MARK: - ResourceCache
namespace vox {
ResourceCache::ResourceCache(wgpu::Device &device) :
_device{device} {
}

wgpu::RenderPipeline *ResourceCache::requestRenderPipeline(wgpu::RenderPipelineDescriptor &descriptor) {
    std::hash<wgpu::RenderPipelineDescriptor> hasher;
    size_t hash = hasher(descriptor);
    
    auto iter = _state.renderPipelines.find(hash);
    if (iter == _state.renderPipelines.end()) {
        auto pipelineState = std::make_unique<wgpu::RenderPipeline>(_device.CreateRenderPipeline(&descriptor));
        _state.renderPipelines[hash] = std::move(pipelineState);
        return _state.renderPipelines[hash].get();
    } else {
        return iter->second.get();
    }
}

ShaderProgram *ResourceCache::requestShader(const std::string &vertexSource,
                                            const std::string &fragmentSource) {
    std::size_t hash{0U};
    hash_combine(hash, std::hash<std::string>{}(vertexSource));
    hash_combine(hash, std::hash<std::string>{}(fragmentSource));
    
    auto iter = _state.shaders.find(hash);
    if (iter == _state.shaders.end()) {
        auto shader = std::make_unique<ShaderProgram>(_device, vertexSource, fragmentSource);
        _state.shaders[hash] = std::move(shader);
        return _state.shaders[hash].get();
    } else {
        return iter->second.get();
    }
}

}
