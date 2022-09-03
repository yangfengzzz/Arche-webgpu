//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <android_native_app_glue.h>

#include "vox.render/platform/platform.h"

namespace vox {
/**
 * @brief Process android lifecycle events
 *
 * @param app Android app context
 * @return true Events processed
 * @return false Program should close
 */
inline bool process_android_events(android_app *app) {
    android_poll_source *source;

    int ident;
    int events;

    while ((ident = ALooper_pollAll(0, nullptr, &events, (void **)&source)) >= 0) {
        if (source) {
            source->process(app, source);
        }

        if (app->destroyRequested != 0) {
            return false;
        }
    }

    return true;
}

class AndroidPlatform : public Platform {
public:
    AndroidPlatform(android_app *app);

    virtual ~AndroidPlatform() = default;

    virtual ExitCode initialize(const std::vector<Plugin *> &plugins) override;

    virtual void terminate(ExitCode code) override;

    virtual const char *get_surface_extension() override;

    /**
     * @brief Sends a notification in the task bar
     * @param message The message to display
     */
    void send_notification(const std::string &message);

    /**
     * @brief Sends an error notification in the task bar
     * @param message The message to display
     */
    void send_error_notification(const std::string &message);

    android_app *get_android_app();

    ANativeActivity *get_activity();

    virtual std::unique_ptr<RenderContext> create_render_context(
            Device &device,
            VkSurfaceKHR surface,
            const std::vector<VkSurfaceFormatKHR> &surface_format_priority) const override;

    void set_surface_ready();

private:
    virtual void create_window(const Window::Properties &properties) override;

private:
    android_app *app{nullptr};

    std::string log_output;

    virtual std::vector<spdlog::sink_ptr> get_platform_sinks() override;

    bool surface_ready{false};
};

}  // namespace vox
