//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/platform/plugins/plugin_base.h"

namespace plugins {
using FileLoggerTags = vox::PluginBase<vox::tags::Passive>;

/**
 * @brief File Logger
 *
 * Enables writing log messages to a file
 *
 * Usage: vulkan_sample --log-file filename.txt
 *
 */
class FileLogger : public FileLoggerTags {
public:
    FileLogger();

    ~FileLogger() override = default;

    bool IsActive(const vox::CommandParser &parser) override;

    void Init(const vox::CommandParser &parser) override;

    vox::FlagCommand log_file_flag_ = {vox::FlagType::ONE_VALUE, "log-file", "",
                                       "Write log messages to the given file name"};
};
}  // namespace plugins
