//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/platform/plugins/plugin_base.h"

namespace plugins {
class WindowOptions;

using WindowOptionsTags = vox::PluginBase<WindowOptions, vox::tags::Passive>;

/**
 * @brief Window Options
 *
 * Configure the window used when running Vulkan Samples.
 *
 * Usage: vulkan_samples sample instancing --width 500 --height 500 --vsync OFF
 *
 */
class WindowOptions : public WindowOptionsTags {
public:
    WindowOptions();

    ~WindowOptions() override = default;

    bool IsActive(const vox::CommandParser &parser) override;

    void Init(const vox::CommandParser &options) override;

    vox::FlagCommand width_flag_ = {vox::FlagType::ONE_VALUE, "width", "", "Initial window width"};
    vox::FlagCommand height_flag_ = {vox::FlagType::ONE_VALUE, "height", "", "Initial window height"};
    vox::FlagCommand fullscreen_flag_ = {vox::FlagType::FLAG_ONLY, "fullscreen", "", "Run in fullscreen mode"};
    vox::FlagCommand headless_flag_ = {vox::FlagType::FLAG_ONLY, "headless", "", "Run in headless mode"};
    vox::FlagCommand borderless_flag_ = {vox::FlagType::FLAG_ONLY, "borderless", "", "Run in borderless mode"};
    vox::FlagCommand vsync_flag_ = {vox::FlagType::ONE_VALUE, "vsync", "",
                                    "Force vsync {ON | OFF}. If not set samples decide how vsync is set"};

    vox::CommandGroup window_options_group_ = {
            "Window Options",
            {&width_flag_, &height_flag_, &vsync_flag_, &fullscreen_flag_, &borderless_flag_, &headless_flag_}};
};
}  // namespace plugins
