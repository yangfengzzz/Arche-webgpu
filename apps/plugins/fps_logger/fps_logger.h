//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/platform/plugins/plugin_base.h"
namespace plugins {
using FpsLoggerTags = vox::PluginBase<vox::tags::Passive>;

/**
 * @brief FPS Logger
 *
 * Control when FPS should be logged. Declutters the log output by removing FPS logs when not enabled
 *
 * Usage: vulkan_sample sample afbc --log-fps
 *
 */
class FpsLogger : public FpsLoggerTags {
public:
    FpsLogger();

    ~FpsLogger() override = default;

    bool IsActive(const vox::CommandParser &parser) override;

    void Init(const vox::CommandParser &parser) override;

    void OnUpdate(float delta_time) override;

    vox::FlagCommand fps_flag_ = {vox::FlagType::FLAG_ONLY, "log-fps", "", "Log FPS"};

private:
    vox::utility::Timer timer_;

    size_t frame_count_{0};

    size_t last_frame_count_{0};
};
}  // namespace plugins
