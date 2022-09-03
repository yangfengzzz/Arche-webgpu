//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/platform/unix/unix_d2d_platform.h"

#include "vox.render/error.h"

VKBP_DISABLE_WARNINGS()

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

VKBP_ENABLE_WARNINGS()

#include "vox.render/platform/headless_window.h"
#include "vox.render/platform/unix/direct_window.h"

namespace vox {
namespace {
inline const std::string get_temp_path_from_environment() {
    std::string temp_path = "/tmp/";

    if (const char *env_ptr = std::getenv("TMPDIR")) {
        temp_path = std::string(env_ptr) + "/";
    }

    return temp_path;
}
}  // namespace

namespace fs {
void CreateDirectory(const std::string &path) {
    if (!is_directory(path)) {
        mkdir(path.c_str(), 0777);
    }
}
}  // namespace fs

UnixD2DPlatform::UnixD2DPlatform(int argc, char **argv) {
    Platform::SetArguments({argv + 1, argv + argc});
    Platform::SetTempDirectory(get_temp_path_from_environment());
}

void UnixD2DPlatform::CreateWindow(const Window::Properties &properties) {
    if (properties.mode == vox::Window::Mode::HEADLESS) {
        window_ = std::make_unique<HeadlessWindow>(properties);
    } else {
        window_ = std::make_unique<DirectWindow>(this, properties);
    }
}

const char *UnixD2DPlatform::GetSurfaceExtension() { return VK_KHR_DISPLAY_EXTENSION_NAME; }

}  // namespace vox
