//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <dawn/native/NullBackend.h>

#include <memory>

#include "backend_binding.h"

namespace vox {
class NullBinding : public BackendBinding {
public:
    NullBinding(GLFWwindow* window, wgpu::Device& device) : BackendBinding(window, device) {}

    uint64_t swapChainImplementation() override {
        if (_swapchainImpl.userData == nullptr) {
            _swapchainImpl = dawn::native::null::CreateNativeSwapChainImpl();
        }
        return reinterpret_cast<uint64_t>(&_swapchainImpl);
    }
    wgpu::TextureFormat preferredSwapChainTextureFormat() override { return wgpu::TextureFormat::RGBA8Unorm; }

private:
    DawnSwapChainImplementation _swapchainImpl = {};
};

std::unique_ptr<BackendBinding> createNullBinding(GLFWwindow* window, wgpu::Device& device) {
    return std::make_unique<NullBinding>(window, device);
}

}  // namespace vox
