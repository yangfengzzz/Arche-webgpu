//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/plugins/fps_logger/fps_logger.h"

namespace plugins {
FpsLogger::FpsLogger()
    : FpsLoggerTags(
              "FPS Logger", "Enable FPS logging.", {vox::Hook::ON_UPDATE, vox::Hook::ON_APP_START}, {&fps_flag_}) {}

bool FpsLogger::IsActive(const vox::CommandParser &parser) { return parser.Contains(&fps_flag_); }

void FpsLogger::Init(const vox::CommandParser &parser) {}

void FpsLogger::OnUpdate(float delta_time) {
    if (!timer_.IsRunning()) {
        timer_.Start();
    }

    auto elapsed_time = static_cast<float>(timer_.Elapsed<vox::utility::Timer::Seconds>());

    frame_count_++;

    if (elapsed_time > 0.5f) {
        auto fps = (frame_count_ - last_frame_count_) / elapsed_time;

        LOGI("FPS: {:.1f}", fps)

        last_frame_count_ = frame_count_;
        timer_.Lap();
    }
}
}  // namespace plugins
