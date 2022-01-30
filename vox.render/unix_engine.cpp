//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "unix_engine.h"

#include "glfw_window.h"
#include "filesystem.h"

namespace vox {
namespace {
inline const std::string get_temp_path_from_environment() {
    std::string temp_path = "/tmp/";
    
    if (const char *env_ptr = std::getenv("TMPDIR")) {
        temp_path = std::string(env_ptr) + "/";
    }
    
    return temp_path;
}
}        // namespace

namespace fs {
void createDirectory(const std::string &path) {
    if (!isDirectory(path)) {
        mkdir(path.c_str(), 0777);
    }
}
}        // namespace fs

UnixEngine::UnixEngine(const UnixType &type, int argc, char **argv) :
_type{type} {
    Engine::setArguments({argv + 1, argv + argc});
    Engine::setTempDirectory(get_temp_path_from_environment());
}

void UnixEngine::createWindow(const Window::Properties &properties) {
    _window = std::make_unique<GlfwWindow>(this, properties);
}
}        // namespace vox
