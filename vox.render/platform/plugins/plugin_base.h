//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <spdlog/fmt/fmt.h>

#include <utility>

#include "vox.render/platform/parser.h"
#include "vox.render/platform/platform.h"
#include "vox.render/platform/plugins/plugin.h"
#include "vox.render/tags.h"

namespace vox {
/**
 * @brief PluginBase is the base class that plugins inherit from. The class enforces the use of tags when creating new
 * plugins. For method information see Plugin
 */
template <typename... TAGS>
class PluginBase : public Plugin, public Tag<TAGS...> {
public:
    PluginBase(std::string name,
               std::string description,
               std::vector<Hook> hooks = {},
               std::vector<Command *> commands = {});

    ~PluginBase() override = default;

    [[nodiscard]] const std::vector<Command *> &GetCliCommands() const override;

    [[nodiscard]] const std::vector<Hook> &GetHooks() const override;

    bool HasTag(TagId id) const override;

    // hooks that can be implemented by plugins
    void OnUpdate(float delta_time) override{};

    void OnAppStart(const std::string &app_id) override{};

    void OnAppClose(const std::string &app_id) override{};

    void OnPlatformClose() override{};

    void OnPostDraw(RenderContext &context) override{};

    void OnAppError(const std::string &app_id) override{};

private:
    Tag<TAGS...> *tags_ = reinterpret_cast<Tag<TAGS...> *>(this);

    std::vector<Hook> hooks_;
    std::vector<Command *> commands_;
};

template <typename... TAGS>
PluginBase<TAGS...>::PluginBase(const std::string name,
                                const std::string description,
                                std::vector<Hook> hooks,
                                std::vector<Command *> commands)
    : Plugin(name, description), hooks_{std::move(hooks)}, commands_{std::move(commands)} {}

template <typename... TAGS>
const std::vector<Command *> &PluginBase<TAGS...>::GetCliCommands() const {
    return commands_;
}

template <typename... TAGS>
bool PluginBase<TAGS...>::HasTag(TagId id) const {
    return tags_->HasTag(id);
}

template <typename... TAGS>
const std::vector<Hook> &PluginBase<TAGS...>::GetHooks() const {
    return hooks_;
}

}  // namespace vox
