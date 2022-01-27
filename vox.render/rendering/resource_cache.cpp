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
struct hash<wgpu::RenderPipelineDescriptor> {
    std::size_t operator()(const wgpu::RenderPipelineDescriptor &descriptor) const {
        std::size_t result = 0;
        
//        hash_combine(result, descriptor.vertexDescriptor()->objCObj()); // internal address
//        hash_combine(result, descriptor.vertexFunction()->objCObj()); // internal address
//        if (descriptor.fragmentFunction()) {
//            hash_combine(result, descriptor.fragmentFunction()->objCObj()); // internal address
//        }
//        hash_combine(result, descriptor.sampleCount());
//        hash_combine(result, descriptor.isAlphaToCoverageEnabled());
//        hash_combine(result, descriptor.depthAttachmentPixelFormat());
//        hash_combine(result, descriptor.stencilAttachmentPixelFormat());
//        if (descriptor.colorAttachments[0].pixelFormat() != MTL::PixelFormatInvalid) {
//            hash_combine(result, descriptor.colorAttachments[0].pixelFormat());
//            hash_combine(result, descriptor.colorAttachments[0].isBlendingEnabled());
//            hash_combine(result, descriptor.colorAttachments[0].sourceRGBBlendFactor());
//            hash_combine(result, descriptor.colorAttachments[0].destinationRGBBlendFactor());
//            hash_combine(result, descriptor.colorAttachments[0].rgbBlendOperation());
//            hash_combine(result, descriptor.colorAttachments[0].sourceAlphaBlendFactor());
//            hash_combine(result, descriptor.colorAttachments[0].destinationAlphaBlendFactor());
//            hash_combine(result, descriptor.colorAttachments[0].alphaBlendOperation());
//            hash_combine(result, descriptor.colorAttachments[0].writeMask());
//        }
        
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
