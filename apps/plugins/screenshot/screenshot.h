//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/platform/filesystem.h"
#include "vox.render/platform/plugins/plugin_base.h"

namespace plugins {
class Screenshot;

using ScreenshotTags = vox::PluginBase<Screenshot, vox::tags::Passive>;

/**
 * @brief Screenshot
 *
 * Capture a screen shot of the last rendered image at a given frame. The output can also be named
 *
 * Usage: vulkan_sample sample afbc --screenshot 1 --screenshot-output afbc-screenshot
 *
 */
class Screenshot : public ScreenshotTags {
public:
    Screenshot();

    ~Screenshot() override = default;

    bool IsActive(const vox::CommandParser &parser) override;

    void Init(const vox::CommandParser &parser) override;

    void OnUpdate(float delta_time) override;

    void OnAppStart(const std::string &app_info) override;

    void OnPostDraw(vox::RenderContext &context) override;

    vox::FlagCommand screenshot_flag_ = {vox::FlagType::ONE_VALUE, "screenshot", "",
                                         "Take a screenshot at a given frame"};
    vox::FlagCommand screenshot_output_flag_ = {vox::FlagType::ONE_VALUE, "screenshot-output", "",
                                                "Declare an output name for the image"};

private:
    uint32_t current_frame_ = 0;
    uint32_t frame_number_{};
    std::string current_app_name_;

    bool output_path_set_ = false;
    std::string output_path_;
};
}  // namespace plugins
