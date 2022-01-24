//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "unix_engine.h"

#include "glfw_window.h"

namespace vox {
UnixEngine::UnixEngine(const UnixType &type, int argc, char **argv) :
_type{type} {
}

void UnixEngine::createWindow(const Window::Properties &properties) {
    _window = std::make_unique<GlfwWindow>(this, properties);
}
}        // namespace vox
