//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/graphics_application.h"

#include <dawn/dawn_proc.h>

#include "vox.render/platform/glfw_window.h"
#include "vox.render/platform/platform.h"

namespace vox {
// Default to D3D12, Metal, Vulkan, OpenGL in that order as D3D12 and Metal are the preferred on
// their respective platforms, and Vulkan is preferred to OpenGL
#if defined(DAWN_ENABLE_BACKEND_D3D12)
static wgpu::BackendType backendType = wgpu::BackendType::D3D12;
#elif defined(DAWN_ENABLE_BACKEND_METAL)
static wgpu::BackendType backendType = wgpu::BackendType::Metal;
#elif defined(DAWN_ENABLE_BACKEND_VULKAN)
static wgpu::BackendType backendType = wgpu::BackendType::Vulkan;
#elif defined(DAWN_ENABLE_BACKEND_OPENGLES)
static wgpu::BackendType backendType = wgpu::BackendType::OpenGLES;
#elif defined(DAWN_ENABLE_BACKEND_DESKTOP_GL)
static wgpu::BackendType backendType = wgpu::BackendType::OpenGL;
#else
#error
#endif

GraphicsApplication::GraphicsApplication() = default;

GraphicsApplication::~GraphicsApplication() {
    _gui.reset();
    _renderContext.reset();
}

bool GraphicsApplication::prepare(Platform& platform) {
    if (!Application::prepare(platform)) {
        return false;
    }

    LOGI("Initializing WebGPU Application")

    _createCppDawnDevice();
    _renderContext = platform.CreateRenderContext(_device);
    _gui = std::make_unique<ui::UiManager>(static_cast<GlfwWindow*>(&platform.GetWindow())->Handle(),
                                           _renderContext.get());
    return true;
}

void GraphicsApplication::update(float delta_time) {}

bool GraphicsApplication::resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) {
    Application::resize(win_width, win_height, fb_width, fb_height);
    _renderContext->resize(fb_width, fb_height);
    return true;
}

void GraphicsApplication::inputEvent(const InputEvent& inputEvent) {}

void GraphicsApplication::finish() {}

void PrintDeviceError(WGPUErrorType errorType, const char* message, void*) {
    const char* errorTypeName = "";
    switch (errorType) {
        case WGPUErrorType_Validation:
            errorTypeName = "Validation";
            break;
        case WGPUErrorType_OutOfMemory:
            errorTypeName = "Out of memory";
            break;
        case WGPUErrorType_Unknown:
            errorTypeName = "Unknown";
            break;
        case WGPUErrorType_DeviceLost:
            errorTypeName = "Device lost";
            break;
        default:
            assert(false && "UNREACHABLE");
            return;
    }
    LOGE("{} error: {}", errorTypeName, message)
}

void GraphicsApplication::_createCppDawnDevice() {
    _instance = std::make_unique<dawn_native::Instance>();
    _instance->DiscoverDefaultAdapters();

    // Get an adapter for the backend to use, and create the device.
    dawn_native::Adapter backendAdapter;
    {
        std::vector<dawn_native::Adapter> adapters = _instance->GetAdapters();
        auto adapterIt =
                std::find_if(adapters.begin(), adapters.end(), [](const dawn_native::Adapter& adapter) -> bool {
                    wgpu::AdapterProperties properties;
                    adapter.GetProperties(&properties);
                    return properties.backendType == backendType;
                });
        assert(adapterIt != adapters.end());
        backendAdapter = *adapterIt;
    }
    WGPUDevice backendDevice = backendAdapter.CreateDevice();
    DawnProcTable backendProcs = dawn_native::GetProcs();
    dawnProcSetProcs(&backendProcs);
    backendProcs.deviceSetUncapturedErrorCallback(backendDevice, PrintDeviceError, nullptr);
    _device = wgpu::Device::Acquire(backendDevice);
}

}  // namespace vox
