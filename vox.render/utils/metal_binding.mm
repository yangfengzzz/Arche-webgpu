// Copyright 2017 The Dawn Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "backend_binding.h"

#include "common/Assert.h"
#include "common/SwapChainUtils.h"
#include "dawn_native/MetalBackend.h"

#define GLFW_EXPOSE_NATIVE_COCOA
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#import <QuartzCore/CAMetalLayer.h>

namespace vox {
class SwapChainImplMTL {
public:
    using WSIContext = DawnWSIContextMetal;
    
    SwapChainImplMTL(id nsWindow) : _nsWindow(nsWindow) {
    }
    
    ~SwapChainImplMTL() {
        [_currentTexture release];
        [_currentDrawable release];
    }
    
    void Init(DawnWSIContextMetal* ctx) {
        _mtlDevice = ctx->device;
        _commandQueue = ctx->queue;
    }
    
    DawnSwapChainError Configure(WGPUTextureFormat format,
                                 WGPUTextureUsage usage,
                                 uint32_t width,
                                 uint32_t height) {
        if (format != WGPUTextureFormat_BGRA8UnormSrgb) {
            return "unsupported format";
        }
        ASSERT(width > 0);
        ASSERT(height > 0);
        
        NSView* contentView = [_nsWindow contentView];
        [contentView setWantsLayer:YES];
        
        CGSize size = {};
        size.width = width;
        size.height = height;
        
        _layer = [CAMetalLayer layer];
        [_layer setDevice:_mtlDevice];
        [_layer setPixelFormat:MTLPixelFormatBGRA8Unorm_sRGB];
        [_layer setDrawableSize:size];
        
        constexpr uint32_t kFramebufferOnlyTextureUsages =
        WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_Present;
        bool hasOnlyFramebufferUsages = !(usage & (~kFramebufferOnlyTextureUsages));
        if (hasOnlyFramebufferUsages) {
            [_layer setFramebufferOnly:YES];
        }
        
        [contentView setLayer:_layer];
        
        return DAWN_SWAP_CHAIN_NO_ERROR;
    }
    
    DawnSwapChainError GetNextTexture(DawnSwapChainNextTexture* nextTexture) {
        [_currentDrawable release];
        _currentDrawable = [_layer nextDrawable];
        [_currentDrawable retain];
        
        [_currentTexture release];
        _currentTexture = _currentDrawable.texture;
        [_currentTexture retain];
        
        nextTexture->texture.ptr = reinterpret_cast<void*>(_currentTexture);
        
        return DAWN_SWAP_CHAIN_NO_ERROR;
    }
    
    DawnSwapChainError Present() {
        id<MTLCommandBuffer> commandBuffer = [_commandQueue commandBuffer];
        [commandBuffer presentDrawable:_currentDrawable];
        [commandBuffer commit];
        
        return DAWN_SWAP_CHAIN_NO_ERROR;
    }
    
private:
    id _nsWindow = nil;
    id<MTLDevice> _mtlDevice = nil;
    id<MTLCommandQueue> _commandQueue = nil;
    
    CAMetalLayer* _layer = nullptr;
    id<CAMetalDrawable> _currentDrawable = nil;
    id<MTLTexture> _currentTexture = nil;
};

class MetalBinding : public BackendBinding {
public:
    MetalBinding(GLFWwindow* window, wgpu::Device& device) : BackendBinding(window, device) {
    }
    
    uint64_t swapChainImplementation() override {
        if (_swapchainImpl.userData == nullptr) {
            _swapchainImpl = CreateSwapChainImplementation(new SwapChainImplMTL(glfwGetCocoaWindow(_window)));
        }
        return reinterpret_cast<uint64_t>(&_swapchainImpl);
    }
    
    wgpu::TextureFormat preferredSwapChainTextureFormat() override {
        return wgpu::TextureFormat::BGRA8UnormSrgb;
    }
    
private:
    DawnSwapChainImplementation _swapchainImpl = {};
};

std::unique_ptr<BackendBinding> createMetalBinding(GLFWwindow* window, wgpu::Device& device) {
    return std::make_unique<MetalBinding>(window, device);
}
}
