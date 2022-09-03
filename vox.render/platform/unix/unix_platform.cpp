//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/platform/unix/unix_platform.h"

#include "vox.render/error.h"
#include "vox.render/platform/glfw_window.h"
#include "vox.render/platform/headless_window.h"

VKBP_DISABLE_WARNINGS()

#include <spdlog/sinks/stdout_color_sinks.h>

VKBP_ENABLE_WARNINGS()

#ifndef VK_MVK_MACOS_SURFACE_EXTENSION_NAME
#define VK_MVK_MACOS_SURFACE_EXTENSION_NAME "VK_MVK_macos_surface"
#endif

#ifndef VK_KHR_XCB_SURFACE_EXTENSION_NAME
#define VK_KHR_XCB_SURFACE_EXTENSION_NAME "VK_KHR_xcb_surface"
#endif

#ifndef VK_EXT_METAL_SURFACE_EXTENSION_NAME
#define VK_EXT_METAL_SURFACE_EXTENSION_NAME "VK_EXT_metal_surface"
#endif

#ifndef VK_KHR_XLIB_SURFACE_EXTENSION_NAME
#define VK_KHR_XLIB_SURFACE_EXTENSION_NAME "VK_KHR_xlib_surface"
#endif

#ifndef VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME
#define VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME "VK_KHR_wayland_surface"
#endif

namespace vox {
namespace {
inline std::string get_temp_path_from_environment() {
    std::string temp_path = "/tmp/";

    if (const char *env_ptr = std::getenv("TMPDIR")) {
        temp_path = std::string(env_ptr) + "/";
    }

    return temp_path;
}
}  // namespace

namespace fs {
void CreateDirectory(const std::string &path) {
    if (!IsDirectory(path)) {
        mkdir(path.c_str(), 0777);
    }
}
}  // namespace fs

UnixPlatform::UnixPlatform(const UnixType &type, int argc, char **argv) : type_{type} {
    Platform::SetArguments({argv + 1, argv + argc});
    Platform::SetTempDirectory(get_temp_path_from_environment());
}

const char *UnixPlatform::GetSurfaceExtension() {
    if (type_ == UnixType::MAC) {
        return VK_EXT_METAL_SURFACE_EXTENSION_NAME;
    }

#if defined(VK_USE_PLATFORM_XCB_KHR)
    return VK_KHR_XCB_SURFACE_EXTENSION_NAME;
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
    return VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
    return VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME;
#else
    assert(0 && "Platform not supported, no surface extension available");
    throw std::runtime_error("");
#endif
}

void UnixPlatform::CreateWindow(const Window::Properties &properties) {
    if (properties.mode == vox::Window::Mode::HEADLESS) {
        window_ = std::make_unique<HeadlessWindow>(properties);
    } else {
        window_ = std::make_unique<GlfwWindow>(this, properties);
    }
}

}  // namespace vox
