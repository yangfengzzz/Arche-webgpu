//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/platform/plugins/plugin.h"

#include "vox.render/platform/platform.h"

namespace vox {
std::vector<Plugin *> AssociatePlugins(const std::vector<Plugin *> &plugins) {
    for (auto *plugin : plugins) {
        for (auto *comparison_plugin : plugins) {
            bool full_control = comparison_plugin->HasTags<tags::FullControl>();
            bool stopping = comparison_plugin->HasTags<tags::Stopping>();
            bool controlling = full_control || stopping;

            bool entrypoint = comparison_plugin->HasTags<tags::Entrypoint>();

            if (plugin->HasTag<tags::FullControl>() && (controlling || entrypoint)) {
                plugin->Excludes(comparison_plugin);
                continue;
            }

            if (plugin->HasTag<tags::Stopping>() && stopping) {
                plugin->Excludes(comparison_plugin);
                continue;
            }

            if (plugin->HasTag<tags::Entrypoint>() && entrypoint) {
                plugin->Excludes(comparison_plugin);
                continue;
            }

            plugin->Includes(comparison_plugin);
        }
    }

    return plugins;
}

bool Plugin::ActivatePlugin(Platform *platform, const CommandParser &parser, bool force_activation) {
    platform_ = platform;

    bool active = IsActive(parser);

    // Plugin activated
    if (force_activation || active) {
        Init(parser);
    }

    return active;
}

const std::string &Plugin::GetName() const { return name_; }

const std::string &Plugin::GetDescription() const { return description_; }

void Plugin::Excludes(Plugin *plugin) { exclusions_.push_back(plugin); }

const std::vector<Plugin *> &Plugin::GetExclusions() const { return exclusions_; }

void Plugin::Includes(Plugin *plugin) { inclusions_.push_back(plugin); }

const std::vector<Plugin *> &Plugin::GetInclusions() const { return inclusions_; }

}  // namespace vox
