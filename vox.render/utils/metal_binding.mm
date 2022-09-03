//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/utils/backend_binding.h"

#include <dawn/native/MetalBackend.h>

#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#import <QuartzCore/CAMetalLayer.h>

namespace vox {
class SwapChainImplMTL {
public:
    using WSIContext = DawnWSIContextMetal;

    SwapChainImplMTL(id nsWindow) : _nsWindow(nsWindow) {}

    ~SwapChainImplMTL() {
        [_currentTexture release];
        [_currentDrawable release];
    }

    void Init(DawnWSIContextMetal* ctx) {
        _mtlDevice = ctx->device;
        _commandQueue = ctx->queue;
    }

    DawnSwapChainError Configure(WGPUTextureFormat format, WGPUTextureUsage usage, uint32_t width, uint32_t height) {
        if (format != WGPUTextureFormat_BGRA8UnormSrgb) {
            return "unsupported format";
        }

        NSView* contentView = [_nsWindow contentView];
        [contentView setWantsLayer:YES];

        CGSize size = {};
        size.width = width;
        size.height = height;

        _layer = [CAMetalLayer layer];
        [_layer setDevice:_mtlDevice];
        [_layer setPixelFormat:MTLPixelFormatBGRA8Unorm_sRGB];
        [_layer setDrawableSize:size];

        constexpr uint32_t kFramebufferOnlyTextureUsages = WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_Present;
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
    MetalBinding(GLFWwindow* window, wgpu::Device& device) : BackendBinding(window, device) {}

    uint64_t swapChainImplementation() override {
        if (_swapchainImpl.userData == nullptr) {
            _swapchainImpl = CreateSwapChainImplementation(new SwapChainImplMTL(glfwGetCocoaWindow(_window)));
        }
        return reinterpret_cast<uint64_t>(&_swapchainImpl);
    }

    wgpu::TextureFormat preferredSwapChainTextureFormat() override { return wgpu::TextureFormat::BGRA8UnormSrgb; }

private:
    template <typename T>
    DawnSwapChainImplementation CreateSwapChainImplementation(T* swapChain) {
        DawnSwapChainImplementation impl = {};
        impl.userData = swapChain;
        impl.Init = [](void* userData, void* wsiContext) {
            auto* ctx = static_cast<typename T::WSIContext*>(wsiContext);
            reinterpret_cast<T*>(userData)->Init(ctx);
        };
        impl.Destroy = [](void* userData) { delete reinterpret_cast<T*>(userData); };
        impl.Configure = [](void* userData, WGPUTextureFormat format, WGPUTextureUsage allowedUsage, uint32_t width,
                            uint32_t height) {
            return static_cast<T*>(userData)->Configure(format, allowedUsage, width, height);
        };
        impl.GetNextTexture = [](void* userData, DawnSwapChainNextTexture* nextTexture) {
            return static_cast<T*>(userData)->GetNextTexture(nextTexture);
        };
        impl.Present = [](void* userData) { return static_cast<T*>(userData)->Present(); };
        return impl;
    }

    DawnSwapChainImplementation _swapchainImpl = {};
};

std::unique_ptr<BackendBinding> createMetalBinding(GLFWwindow* window, wgpu::Device& device) {
    return std::make_unique<MetalBinding>(window, device);
}
}  // namespace vox
