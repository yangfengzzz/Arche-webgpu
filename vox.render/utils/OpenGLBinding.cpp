//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <cstdio>

#include "common/Assert.h"
#include "common/Platform.h"
#include "common/SwapChainUtils.h"
#include "dawn/dawn_wsi.h"
#include "dawn_native/OpenGLBackend.h"
#include "GLFW/glfw3.h"
#include "utils/BackendBinding.h"

namespace utils {

class OpenGLBinding : public BackendBinding {
public:
    OpenGLBinding(GLFWwindow* window, WGPUDevice device) : BackendBinding(window, device) {}

    uint64_t GetSwapChainImplementation() override {
        if (mSwapchainImpl.userData == nullptr) {
            mSwapchainImpl = dawn::native::opengl::CreateNativeSwapChainImpl(
                    mDevice, [](void* userdata) { glfwSwapBuffers(static_cast<GLFWwindow*>(userdata)); }, mWindow);
        }
        return reinterpret_cast<uint64_t>(&mSwapchainImpl);
    }

    WGPUTextureFormat GetPreferredSwapChainTextureFormat() override {
        return dawn::native::opengl::GetNativeSwapChainPreferredFormat(&mSwapchainImpl);
    }

private:
    DawnSwapChainImplementation mSwapchainImpl = {};
};

BackendBinding* CreateOpenGLBinding(GLFWwindow* window, WGPUDevice device) { return new OpenGLBinding(window, device); }

}  // namespace utils
