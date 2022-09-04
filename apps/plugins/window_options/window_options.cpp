//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/plugins/window_options/window_options.h"

#include <algorithm>

#include "vox.render/platform/platform.h"
#include "vox.render/platform/window.h"

namespace plugins {
WindowOptions::WindowOptions()
    : WindowOptionsTags("Window Options",
                        "A collection of flags to configure window used when running the application. Implementation "
                        "may differ between platforms",
                        {},
                        {&window_options_group_}) {}

bool WindowOptions::IsActive(const vox::CommandParser &parser) { return true; }

void WindowOptions::Init(const vox::CommandParser &parser) {
    vox::Window::OptionalProperties properties;

    if (parser.Contains(&width_flag_)) {
        auto width = parser.As<uint32_t>(&width_flag_);
        if (width < vox::Platform::min_window_width_) {
            LOGD("[Window Options] {} is smaller than the minimum width {}, resorting to minimum width", width,
                 platform_->min_window_width_)
            width = vox::Platform::min_window_width_;
        }
        properties.extent.width = width;
    }

    if (parser.Contains(&height_flag_)) {
        auto height = parser.As<uint32_t>(&height_flag_);
        if (height < vox::Platform::min_window_height_) {
            LOGD("[Window Options] {} is smaller than the minimum height {}, resorting to minimum height", height,
                 platform_->min_window_height_)
            height = vox::Platform::min_window_height_;
        }
        properties.extent.height = height;
    }

    if (parser.Contains(&headless_flag_)) {
        properties.mode = vox::Window::Mode::HEADLESS;
    } else if (parser.Contains(&fullscreen_flag_)) {
        properties.mode = vox::Window::Mode::FULLSCREEN;
    } else if (parser.Contains(&borderless_flag_)) {
        properties.mode = vox::Window::Mode::FULLSCREEN_BORDERLESS;
    }

    if (parser.Contains(&vsync_flag_)) {
        auto value = parser.As<std::string>(&vsync_flag_);
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        if (value == "on") {
            properties.vsync = vox::Window::Vsync::ON;
        } else if (value == "off") {
            properties.vsync = vox::Window::Vsync::OFF;
        }
    }

    platform_->SetWindowProperties(properties);
}
}  // namespace plugins
