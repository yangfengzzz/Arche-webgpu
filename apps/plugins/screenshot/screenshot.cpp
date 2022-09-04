//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/plugins/screenshot/screenshot.h"

#include <chrono>
#include <iomanip>

#include "vox.render/rendering/render_context.h"

namespace plugins {
Screenshot::Screenshot()
    : ScreenshotTags("Screenshot",
                     "Save a screenshot of a specific frame",
                     {vox::Hook::ON_UPDATE, vox::Hook::ON_APP_START, vox::Hook::POST_DRAW},
                     {&screenshot_flag_, &screenshot_output_flag_}) {}

bool Screenshot::IsActive(const vox::CommandParser &parser) { return parser.Contains(&screenshot_flag_); }

void Screenshot::Init(const vox::CommandParser &parser) {
    if (parser.Contains(&screenshot_flag_)) {
        frame_number_ = parser.As<uint32_t>(&screenshot_flag_);

        if (parser.Contains(&screenshot_output_flag_)) {
            output_path_ = parser.As<std::string>(&screenshot_output_flag_);
            output_path_set_ = true;
        }
    }
}

void Screenshot::OnUpdate(float delta_time) { current_frame_++; }

void Screenshot::OnAppStart(const std::string &name) {
    current_app_name_ = name;
    current_frame_ = 0;
}

void Screenshot::OnPostDraw(vox::RenderContext &context) {
    if (current_frame_ == frame_number_) {
        if (!output_path_set_) {
            // Create generic image path. <app name>-<current timestamp>.png
            auto timestamp = std::chrono::system_clock::now();
            std::time_t now_tt = std::chrono::system_clock::to_time_t(timestamp);
            std::tm tm = *std::localtime(&now_tt);

            char buffer[30];
            strftime(buffer, sizeof(buffer), "%G-%m-%d---%H-%M-%S", &tm);

            std::stringstream stream;
            stream << current_app_name_ << "-" << buffer;

            output_path_ = stream.str();
        }

        vox::Screenshot(context, output_path_);
    }
}
}  // namespace plugins
