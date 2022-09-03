//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "vox.render/core/device.h"
#include "vox.render/platform/application.h"
#include "vox.render/platform/filesystem.h"
#include "vox.render/platform/parser.h"
#include "vox.render/platform/plugins/plugin.h"
#include "vox.render/platform/window.h"
#include "vox.render/rendering/render_context.h"
#include "vox.render/utils.h"
#include "vox.render/vk_common.h"

#if defined(VK_USE_PLATFORM_XLIB_KHR)
#undef Success
#endif

namespace vox {
enum class ExitCode {
    SUCCESS = 0, /* App executed as expected */
    HELP,        /* App should show help */
    CLOSE,       /* App has been requested to close at initialization */
    FATAL_ERROR  /* App encountered an unexpected error */
};

class Platform {
public:
    Platform() = default;

    virtual ~Platform() = default;

    /**
     * @brief Initialize the platform
     * @param plugins plugins available to the platform
     * @return An exit code representing the outcome of initialization
     */
    virtual ExitCode Initialize(std::vector<Plugin *> plugins = {});

    /**
     * @brief Handles the main loop of the platform
     * This should be overridden if a platform requires a specific main loop setup.
     * @return An exit code representing the outcome of the loop
     */
    ExitCode MainLoop();

    /**
     * @brief Runs the application for one frame
     */
    void Update();

    /**
     * @brief Terminates the platform and the application
     * @param code Determines how the platform should exit
     */
    virtual void Terminate(ExitCode code);

    /**
     * @brief Requests to close the platform at the next available point
     */
    virtual void Close();

    virtual std::unique_ptr<RenderContext> CreateRenderContext(
            Device &device, VkSurfaceKHR surface, const std::vector<VkSurfaceFormatKHR> &surface_format_priority) const;

    virtual void Resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height);

    virtual void InputEvent(const InputEvent &input_event);

public:
    Window &GetWindow();

    [[nodiscard]] Application &GetApp() const;

    Application &GetApp();

    void SetApp(std::unique_ptr<Application> &&active_app);

    bool StartApp();

public:
    void SetFocus(bool focused);

    void ForceSimulationFps(float fps);

    void DisableInputProcessing();

public:
    void SetWindowProperties(const Window::OptionalProperties &properties);

    /**
     * @brief Returns the working directory of the application set by the platform
     * @returns The path to the working directory
     */
    static const std::string &GetExternalStorageDirectory();

    /**
     * @brief Returns the suitable directory for temporary files from the environment variables set in the system
     * @returns The path to the temp folder on the system
     */
    static const std::string &GetTempDirectory();

    static std::vector<std::string> &GetArguments();

    static void SetArguments(const std::vector<std::string> &args);

    static void SetExternalStorageDirectory(const std::string &dir);

    static void SetTempDirectory(const std::string &dir);

    static const uint32_t min_window_width_;
    static const uint32_t min_window_height_;

public:
    /**
     * @return The VkInstance extension name for the platform
     */
    virtual const char *GetSurfaceExtension() = 0;

    template <class T>
    T *GetPlugin() const;

    template <class T>
    [[nodiscard]] bool UsingPlugin() const;

    void OnPostDraw(RenderContext &context);

protected:
    std::unique_ptr<CommandParser> parser_;

    std::vector<Plugin *> active_plugins_;

    std::unordered_map<Hook, std::vector<Plugin *>> hooks_;

    std::unique_ptr<Window> window_{nullptr};

    std::unique_ptr<Application> active_app_{nullptr};

    virtual std::vector<spdlog::sink_ptr> GetPlatformSinks();

    /**
     * @brief Handles the creation of the window
     *
     * @param properties Preferred window configuration
     */
    virtual void CreateWindow(const Window::Properties &properties) = 0;

    void OnUpdate(float delta_time);

    void OnAppError(const std::string &app_id);

    void OnAppStart(const std::string &app_id);

    void OnAppClose(const std::string &app_id);

    void OnPlatformClose();

    Window::Properties window_properties_; /* Source of truth for window state */
    bool fixed_simulation_fps_{false};     /* Delta time should be fixed with a fabricated value */
    float simulation_frame_time_ = 0.016f; /* A fabricated delta time */
    bool process_input_events_{true};      /* App should continue processing input events */
    bool focused_{true};                   /* App is currently in focus at an operating system level */
    bool close_requested_{false};          /* Close requested */

private:
    utility::Timer timer_;

    std::vector<Plugin *> plugins_;

    std::vector<std::unique_ptr<Plugin>> internal_plugins_;

    /// Static so can be set via JNI code in android_platform.cpp
    static std::vector<std::string> arguments_;

    static std::string external_storage_directory_;

    static std::string temp_directory_;
};

template <class T>
bool Platform::UsingPlugin() const {
    return !plugins::WithTags<T>(active_plugins_).empty();
}

template <class T>
T *Platform::GetPlugin() const {
    assert(UsingPlugin<T>() && "Plugin is not enabled but was requested");
    const auto kPlugins = plugins::WithTags<T>(active_plugins_);
    return dynamic_cast<T *>(kPlugins[0]);
}

}  // namespace vox
