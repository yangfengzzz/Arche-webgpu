//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/platform/application.h"

#include "vox.base/logging.h"
#include "vox.render/platform/platform.h"

namespace vox {
Application::Application() : name_{"Sample Name"} {}

bool Application::Prepare(Platform &platform) {
    auto &debug_info = GetDebugInfo();
    debug_info.insert<field::MinMax, float>("fps", fps_);
    debug_info.insert<field::MinMax, float>("frame_time", frame_time_);

    platform_ = &platform;

    return true;
}

void Application::Finish() {}

bool Application::Resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) {
    return true;
}

void Application::InputEvent(const vox::InputEvent &input_event) {}

void Application::Update(float delta_time) {
    fps_ = 1.0f / delta_time;
    frame_time_ = delta_time * 1000.0f;
}

const std::string &Application::GetName() const { return name_; }

void Application::SetName(const std::string &name) { name_ = name; }

DebugInfo &Application::GetDebugInfo() { return debug_info_; }

}  // namespace vox
