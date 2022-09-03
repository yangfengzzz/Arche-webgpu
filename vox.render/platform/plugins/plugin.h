//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cassert>
#include <string>
#include <typeindex>
#include <utility>
#include <vector>

#include "vox.render/platform/parser.h"
#include "vox.render/tags.h"

namespace vox {
class Platform;

class RenderContext;

class Plugin;

/**
 * @brief Tags are used to define a plugins behaviour. This is useful to dictate which plugins will work together
 * 	      and which will not without directly specifying an exclusion or inclusion list. Tags are struct types so
 * that they can be used in the tagging system (See plugin implementation).
 *
 * Entrypoint - An entrypoint is a starting point for the application that will load a vox::Application (see StartApp)
 * FullControl - The plugin wants full control over how the application executes. Stopping plugins will be ignored (see
 * batch_mode) Stopping - The plugin will stop the app through its own mechanism (see stop_after) Passive - These
 * plugins provide non intrusive behaviour (see fps_logger)
 */
namespace tags {
struct Entrypoint {};
struct FullControl {};
struct Stopping {};
struct Passive {};
}  // namespace tags

/**
 * @brief Associate how plugins can interact with each other. This interoperability is decided by comparing tags of
 * different plugins. The plugins inclusion and exclusion lists are populated by this function
 *
 * @param plugins A list of plugins which are used together
 * @return std::vector<Plugin *> A list of plugins which are used together
 */
std::vector<Plugin *> AssociatePlugins(const std::vector<Plugin *> &plugins);

/**
 * @brief Hooks are points in the project that an plugin can subscribe too. These can be expanded on to implement more
 * behaviour in the future
 *
 * ON_UPDATE - Executed at each update() loop
 * ON_APP_START - Executed when an app starts
 * ON_APP_CLOSE - Executed when an app closes
 * ON_PLATFORM_CLOSE - Executed when the platform closes (End off the apps lifecycle)
 */
enum class Hook { ON_UPDATE, ON_APP_START, ON_APP_CLOSE, ON_APP_ERROR, ON_PLATFORM_CLOSE, POST_DRAW };

/**
 * @brief Plugins are used to define custom behaviour. This allows the addition of features without directly
 * 		  interfering with the applications core implementation
 */
class Plugin {
public:
    Plugin(std::string name, std::string description) : name_{std::move(name)}, description_{std::move(description)} {};

    virtual ~Plugin() = default;

    /**
     * @brief Conducts the process of activating and initializing an plugin
     *
     * @param platform The platform
     * @param parser The parser used to check if the plugins flags are present
     * @param force_activation Force a plugin to be activated, not advised unless the plugin works without inputs
     * @return true If the plugin is to be activated
     * @return false If the plugin is not active
     */
    bool ActivatePlugin(Platform *platform, const CommandParser &parser, bool force_activation = false);

    [[nodiscard]] virtual const std::vector<Command *> &GetCliCommands() const = 0;

    /**
     * @brief Return a list of hooks that an plugin wants to subscribe to
     *
     * @return Hooks that the plugin wants to use
     */
    [[nodiscard]] virtual const std::vector<Hook> &GetHooks() const = 0;

    /**
     * @brief Called when an application has been updated
     *
     * @param delta_time The time taken to compute a frame
     */
    virtual void OnUpdate(float delta_time) = 0;

    /**
     * @brief Called when an app has started
     *
     * @param app_id The ID of the app
     */
    virtual void OnAppStart(const std::string &app_id) = 0;

    /**
     * @brief Called when an app has been closed
     *
     * @param app_id The ID of the app
     */
    virtual void OnAppClose(const std::string &app_id) = 0;

    /**
     * @brief Handle when an application errors
     *
     * @param app_id The ID of the app which errored
     */
    virtual void OnAppError(const std::string &app_id) = 0;

    /**
     * @brief Called when the platform has been requested to close
     */
    virtual void OnPlatformClose() = 0;

    /**
     * @brief Post Draw
     */
    virtual void OnPostDraw(RenderContext &context) = 0;

    [[nodiscard]] const std::string &GetName() const;

    [[nodiscard]] const std::string &GetDescription() const;

    void Excludes(Plugin *plugin);

    [[nodiscard]] const std::vector<Plugin *> &GetExclusions() const;

    void Includes(Plugin *plugin);

    [[nodiscard]] const std::vector<Plugin *> &GetInclusions() const;

    /**
     * @brief Test whether the plugin contains a given tag
     *
     * @tparam C the tag to check for
     * @return true tag present
     * @return false tag not present
     */
    template <typename C>
    [[nodiscard]] bool HasTag() const {
        return HasTag(Tag<C>::id_);
    }

    /**
     * @brief Tests whether the plugins contains multiple tags
     *
     * @tparam C A set of tags
     * @return true Contains all tags
     * @return false Does not contain all tags
     */
    template <typename... C>
    [[nodiscard]] bool HasTags() const {
        std::vector<TagId> query = {Tag<C>::id_...};
        bool res = true;
        for (auto id : query) {
            res &= HasTag(id);
        }
        return res;
    }

    /**
     * @brief Implemented by plugin base to return if the plugin contains a tag
     *
     * @param id The tag id of a tag
     * @return true contains tag
     * @return false does not contain tag
     */
    virtual bool HasTag(TagId id) const = 0;

protected:
    /**
     * @brief An plugin will override this method so that it can check if it will be activated
     *
     * @param parser A parser that has parsed the command line arguments when the app starts
     * @return true If the plugin should be activated
     * @return false If the plugin should be ignored
     */
    virtual bool IsActive(const CommandParser &parser) = 0;

    /**
     * @brief Sets up an plugin by using values from the parser
     *
     * @param parser The parser
     */
    virtual void Init(const CommandParser &parser) = 0;

    Platform *platform_ = nullptr;

private:
    std::string name_;
    std::string description_;

    std::vector<Plugin *> exclusions_;
    std::vector<Plugin *> inclusions_;
};

/**
 * The following section provides helper functions for filtering containers of plugins
 */
namespace plugins {
/**
 * @brief Get all plugins with tags
 * 		  Plugin must include one or more tags
 *
 * @tparam TAGS Tags that an plugin must contain
 * @param domain The list of plugins to query
 * @return const std::vector<Plugin *> A list of plugins containing one or more TAGS
 */
template <typename... TAGS>
std::vector<Plugin *> WithTags(const std::vector<Plugin *> &domain = {}) {
    std::vector<TagId> tags = {Tag<TAGS>::id_...};
    std::vector<Plugin *> compatible;
    for (auto ext : domain) {
        assert(ext != nullptr);

        bool has_one = false;
        for (auto t : tags) {
            has_one |= ext->HasTag(t);
        }

        if (has_one) {
            compatible.push_back(ext);
        }
    }
    return compatible;
}

/**
 * @brief Get all plugins without the given tags
 * 		  Plugin must not include one or more tags
 * 		  Essentially the opposite of plugins::WithTags<...TAGS>()
 *
 * @tparam TAGS Tags that an plugin must not contain
 * @param domain The list of plugins to query
 * @return const std::vector<Plugin *> A list of plugins containing one or more TAGS
 */
template <typename... TAGS>
std::vector<Plugin *> WithoutTags(const std::vector<Plugin *> &domain = {}) {
    std::vector<TagId> tags = {Tag<TAGS>::id_...};
    std::vector<Plugin *> compatible;
    for (auto ext : domain) {
        assert(ext != nullptr);

        bool has_any = false;
        for (auto t : tags) {
            has_any |= ext->HasTag(t);
        }

        if (!has_any) {
            compatible.push_back(ext);
        }
    }
    return compatible;
}

}  // namespace plugins
}  // namespace vox
