//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "common/Assert.h"
#include "dawn_native/VulkanBackend.h"
#include "utils/BackendBinding.h"

// Include GLFW after VulkanBackend so that it declares the Vulkan-specific functions
#include <memory>

#include "GLFW/glfw3.h"

namespace utils {

class VulkanBinding : public BackendBinding {
public:
    VulkanBinding(GLFWwindow* window, WGPUDevice device) : BackendBinding(window, device) {}

    uint64_t GetSwapChainImplementation() override {
        if (mSwapchainImpl.userData == nullptr) {
            VkSurfaceKHR surface = VK_NULL_HANDLE;
            if (glfwCreateWindowSurface(dawn::native::vulkan::GetInstance(mDevice), mWindow, nullptr, &surface) !=
                VK_SUCCESS) {
                ASSERT(false);
            }

            mSwapchainImpl = dawn::native::vulkan::CreateNativeSwapChainImpl(mDevice, surface);
        }
        return reinterpret_cast<uint64_t>(&mSwapchainImpl);
    }
    WGPUTextureFormat GetPreferredSwapChainTextureFormat() override {
        ASSERT(mSwapchainImpl.userData != nullptr);
        return dawn::native::vulkan::GetNativeSwapChainPreferredFormat(&mSwapchainImpl);
    }

private:
    DawnSwapChainImplementation mSwapchainImpl = {};
};

BackendBinding* CreateVulkanBinding(GLFWwindow* window, WGPUDevice device) { return new VulkanBinding(window, device); }

}  // namespace utils
