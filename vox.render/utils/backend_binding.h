//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <dawn/native/DawnNative.h>
#include <webgpu/webgpu_cpp.h>

struct GLFWwindow;

namespace vox {
class BackendBinding {
public:
    virtual ~BackendBinding() = default;

    virtual uint64_t swapChainImplementation() = 0;

    virtual wgpu::TextureFormat preferredSwapChainTextureFormat() = 0;

    wgpu::Device& device();

protected:
    BackendBinding(GLFWwindow* window, wgpu::Device& device);

    GLFWwindow* _window = nullptr;
    wgpu::Device& _device;
};

void discoverAdapter(dawn::native::Instance* instance, GLFWwindow* window, wgpu::BackendType type);
std::unique_ptr<BackendBinding> createBinding(wgpu::BackendType type, GLFWwindow* window, wgpu::Device& device);

}  // namespace vox