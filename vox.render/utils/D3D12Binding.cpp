//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "common/Assert.h"
#include "dawn_native/D3D12Backend.h"
#include "GLFW/glfw3.h"
#include "utils/BackendBinding.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <memory>

#include "GLFW/glfw3native.h"

namespace utils {

class D3D12Binding : public BackendBinding {
public:
    D3D12Binding(GLFWwindow* window, WGPUDevice device) : BackendBinding(window, device) {}

    uint64_t GetSwapChainImplementation() override {
        if (mSwapchainImpl.userData == nullptr) {
            HWND win32Window = glfwGetWin32Window(mWindow);
            mSwapchainImpl = dawn::native::d3d12::CreateNativeSwapChainImpl(mDevice, win32Window);
        }
        return reinterpret_cast<uint64_t>(&mSwapchainImpl);
    }

    WGPUTextureFormat GetPreferredSwapChainTextureFormat() override {
        ASSERT(mSwapchainImpl.userData != nullptr);
        return dawn::native::d3d12::GetNativeSwapChainPreferredFormat(&mSwapchainImpl);
    }

private:
    DawnSwapChainImplementation mSwapchainImpl = {};
};

BackendBinding* CreateD3D12Binding(GLFWwindow* window, WGPUDevice device) { return new D3D12Binding(window, device); }

}  // namespace utils
