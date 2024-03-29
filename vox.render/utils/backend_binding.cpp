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

#include <GLFW/glfw3.h>

#if defined(DAWN_ENABLE_BACKEND_OPENGL)
#include <dawn/native/OpenGLBackend.h>
#endif  // defined(DAWN_ENABLE_BACKEND_OPENGL)

namespace vox {
#if defined(DAWN_ENABLE_BACKEND_D3D12)
std::unique_ptr<BackendBinding> createD3D12Binding(GLFWwindow* window, wgpu::Device& device);
#endif
#if defined(DAWN_ENABLE_BACKEND_METAL)
std::unique_ptr<BackendBinding> createMetalBinding(GLFWwindow* window, wgpu::Device& device);
#endif
#if defined(DAWN_ENABLE_BACKEND_NULL)
std::unique_ptr<BackendBinding> createNullBinding(GLFWwindow* window, wgpu::Device& device);
#endif
#if defined(DAWN_ENABLE_BACKEND_OPENGL)
std::unique_ptr<BackendBinding> createOpenGLBinding(GLFWwindow* window, wgpu::Device& device);
#endif
#if defined(DAWN_ENABLE_BACKEND_VULKAN)
std::unique_ptr<BackendBinding> createVulkanBinding(GLFWwindow* window, wgpu::Device& device);
#endif

BackendBinding::BackendBinding(GLFWwindow* window, wgpu::Device& device) : _window(window), _device(device) {}

wgpu::Device& BackendBinding::device() { return _device; }

void discoverAdapter(dawn::native::Instance* instance, GLFWwindow* window, wgpu::BackendType type) {
    if (type == wgpu::BackendType::OpenGL || type == wgpu::BackendType::OpenGLES) {
#if defined(DAWN_ENABLE_BACKEND_OPENGL)
        glfwMakeContextCurrent(window);
        auto getProc = reinterpret_cast<void* (*)(const char*)>(glfwGetProcAddress);
        if (type == wgpu::BackendType::OpenGL) {
            dawn::native::opengl::AdapterDiscoveryOptions adapterOptions;
            adapterOptions.getProc = getProc;
            instance->DiscoverAdapters(&adapterOptions);
        } else {
            dawn::native::opengl::AdapterDiscoveryOptionsES adapterOptions;
            adapterOptions.getProc = getProc;
            instance->DiscoverAdapters(&adapterOptions);
        }
#endif  // defined(DAWN_ENABLE_BACKEND_OPENGL)
    } else {
        instance->DiscoverDefaultAdapters();
    }
}

std::unique_ptr<BackendBinding> createBinding(wgpu::BackendType type, GLFWwindow* window, wgpu::Device& device) {
    switch (type) {
#if defined(DAWN_ENABLE_BACKEND_D3D12)
        case wgpu::BackendType::D3D12:
            return createD3D12Binding(window, device);
#endif

#if defined(DAWN_ENABLE_BACKEND_METAL)
        case wgpu::BackendType::Metal:
            return createMetalBinding(window, device);
#endif

#if defined(DAWN_ENABLE_BACKEND_NULL)
        case wgpu::BackendType::Null:
            return createNullBinding(window, device);
#endif

#if defined(DAWN_ENABLE_BACKEND_DESKTOP_GL)
        case wgpu::BackendType::OpenGL:
            return createOpenGLBinding(window, device);
#endif

#if defined(DAWN_ENABLE_BACKEND_OPENGLES)
        case wgpu::BackendType::OpenGLES:
            return createOpenGLBinding(window, device);
#endif

#if defined(DAWN_ENABLE_BACKEND_VULKAN)
        case wgpu::BackendType::Vulkan:
            return createVulkanBinding(window, device);
#endif

        default:
            return nullptr;
    }
}

}  // namespace vox
