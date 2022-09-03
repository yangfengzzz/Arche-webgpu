//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/platform/plugins/plugin_base.h"

namespace vox::plugins {
class ForceClose;

// Passive behaviour
using ForceCloseTags = PluginBase<ForceClose, tags::Passive>;

/**
 * @brief Force Close
 *
 * Force the close of the application if halted before exiting
 *
 * The plugin is used as a boolean with platform->UsingPlugin<ForceClose>();
 *
 * Usage: vulkan_sample sample afbc --force-close
 *
 */
class ForceClose : public ForceCloseTags {
public:
    ForceClose();

    ~ForceClose() override = default;

    bool IsActive(const CommandParser &parser) override;

    void Init(const CommandParser &parser) override;

    FlagCommand stop_cmd_ = {FlagType::FLAG_ONLY, "force-close", "",
                             "Force the close of the application if halted before exiting"};
};

}  // namespace vox::plugins
