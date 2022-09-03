//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/platform/window.h"

#include <utility>

#include "vox.render/platform/platform.h"

namespace vox {
Window::Window(Properties properties) : properties_{std::move(properties)} {}

void Window::ProcessEvents() {}

Window::Extent Window::Resize(const Extent &new_extent) {
    if (properties_.resizable) {
        properties_.extent.width = new_extent.width;
        properties_.extent.height = new_extent.height;
    }

    return properties_.extent;
}

const Window::Extent &Window::GetExtent() const { return properties_.extent; }

float Window::GetContentScaleFactor() const { return 1.0f; }

Window::Mode Window::GetWindowMode() const { return properties_.mode; }

}  // namespace vox
