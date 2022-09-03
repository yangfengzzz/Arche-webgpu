//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/platform/platform.h"

#include <spdlog/async_logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <vector>

#include "vox.base/logging.h"
#include "vox.render/platform/parsers/CLI11.h"
#include "vox.render/platform/plugins/plugin.h"
#include "vox.render/plugins/force_close.h"

namespace vox {
const uint32_t Platform::min_window_width_ = 420;
const uint32_t Platform::min_window_height_ = 320;

std::vector<std::string> Platform::arguments_ = {};

std::string Platform::external_storage_directory_;

std::string Platform::temp_directory_;

ExitCode Platform::Initialize(std::vector<Plugin *> plugins) {
    internal_plugins_.emplace_back(std::make_unique<plugins::ForceClose>());
    for (const auto &internal_plugin : internal_plugins_) {
        plugins.emplace_back(internal_plugin.get());
    }

    auto sinks = GetPlatformSinks();

    auto logger = std::make_shared<spdlog::logger>("logger", sinks.begin(), sinks.end());

#ifdef VKB_DEBUG
    logger->set_level(spdlog::level::debug);
#else
    logger->set_level(spdlog::level::info);
#endif

    logger->set_pattern(LOGGER_FORMAT);
    spdlog::set_default_logger(logger);

    LOGI("Logger initialized")

    parser_ = std::make_unique<Cli11CommandParser>(
            "vulkan_samples",
            "\n\tVulkan Samples\n\n\t\tA collection of samples to demonstrate the Vulkan best practice.\n", arguments_);

    // Process command line arguments
    if (!parser_->Parse(AssociatePlugins(plugins))) {
        return ExitCode::HELP;
    }

    // Subscribe plugins to requested hooks and store activated plugins
    for (auto *plugin : plugins) {
        if (plugin->ActivatePlugin(this, *parser_)) {
            auto &plugin_hooks = plugin->GetHooks();
            for (auto hook : plugin_hooks) {
                auto it = hooks_.find(hook);

                if (it == hooks_.end()) {
                    auto r = hooks_.emplace(hook, std::vector<Plugin *>{});

                    if (r.second) {
                        it = r.first;
                    }
                }

                it->second.emplace_back(plugin);
            }

            active_plugins_.emplace_back(plugin);
        }
    }

    // Platform has been closed by a plugin initialization phase
    if (close_requested_) {
        return ExitCode::CLOSE;
    }

    CreateWindow(window_properties_);

    if (!window_) {
        LOGE("Window creation failed!")
        return ExitCode::FATAL_ERROR;
    }

    return ExitCode::SUCCESS;
}

ExitCode Platform::MainLoop() {
    // Load the requested app
    if (!StartApp()) {
        LOGE("Failed to load requested application")
        return ExitCode::FATAL_ERROR;
    }

    // Compensate for load times of the app by rendering the first frame pre-emptively
    timer_.Tick<utility::Timer::Seconds>();
    active_app_->Update(0.01667f);

    while (!window_->ShouldClose() && !close_requested_) {
        try {
            Update();

            window_->ProcessEvents();
        } catch (std::exception &e) {
            LOGE("Error Message: {}", e.what())
            LOGE("Failed when running application {}", active_app_->GetName())

            OnAppError(active_app_->GetName());
            return ExitCode::FATAL_ERROR;
        }
    }

    return ExitCode::SUCCESS;
}

void Platform::Update() {
    auto delta_time = static_cast<float>(timer_.Tick<utility::Timer::Seconds>());

    if (focused_) {
        OnUpdate(delta_time);

        if (fixed_simulation_fps_) {
            delta_time = simulation_frame_time_;
        }

        active_app_->Update(delta_time);
    }
}

std::unique_ptr<RenderContext> Platform::CreateRenderContext(
        Device &device, VkSurfaceKHR surface, const std::vector<VkSurfaceFormatKHR> &surface_format_priority) const {
    assert(!surface_format_priority.empty() &&
           "Surface format priority list must contain at least one preferred surface format");

    auto extent = window_->GetExtent();
    auto context = std::make_unique<RenderContext>(device, surface, extent.width, extent.height);

    context->SetSurfaceFormatPriority(surface_format_priority);

    context->RequestImageFormat(surface_format_priority[0].format);

    context->SetPresentModePriority({
            VK_PRESENT_MODE_MAILBOX_KHR,
            VK_PRESENT_MODE_FIFO_KHR,
            VK_PRESENT_MODE_IMMEDIATE_KHR,
    });

    switch (window_properties_.vsync) {
        case Window::Vsync::ON:
            context->RequestPresentMode(VK_PRESENT_MODE_FIFO_KHR);
            break;
        case Window::Vsync::OFF:
        default:
            context->RequestPresentMode(VK_PRESENT_MODE_MAILBOX_KHR);
            break;
    }

    return context;
}

void Platform::Terminate(ExitCode code) {
    if (code == ExitCode::HELP) {
        auto help = parser_->Help();
        for (auto &line : help) {
            LOGI(line)
        }
    }

    if (active_app_) {
        std::string id = active_app_->GetName();

        OnAppClose(id);

        active_app_->Finish();
    }

    active_app_.reset();
    window_.reset();

    spdlog::drop_all();

    OnPlatformClose();

    // Halt on all unsuccessful exit codes unless ForceClose is in use
    if (code != ExitCode::SUCCESS && !UsingPlugin<plugins::ForceClose>()) {
#ifndef ANDROID
        std::cout << "Press any key to continue";
        std::cin.get();
#endif
    }
}

void Platform::Close() {
    if (window_) {
        window_->Close();
    }

    // Fallback incase a window is not yet in use
    close_requested_ = true;
}

void Platform::ForceSimulationFps(float fps) {
    fixed_simulation_fps_ = true;
    simulation_frame_time_ = 1 / fps;
}

void Platform::DisableInputProcessing() { process_input_events_ = false; }

void Platform::SetFocus(bool focused) { focused_ = focused; }

void Platform::SetWindowProperties(const Window::OptionalProperties &properties) {
    window_properties_.title = properties.title.has_value() ? properties.title.value() : window_properties_.title;
    window_properties_.mode = properties.mode.has_value() ? properties.mode.value() : window_properties_.mode;
    window_properties_.resizable =
            properties.resizable.has_value() ? properties.resizable.value() : window_properties_.resizable;
    window_properties_.vsync = properties.vsync.has_value() ? properties.vsync.value() : window_properties_.vsync;
    window_properties_.extent.width =
            properties.extent.width.has_value() ? properties.extent.width.value() : window_properties_.extent.width;
    window_properties_.extent.height =
            properties.extent.height.has_value() ? properties.extent.height.value() : window_properties_.extent.height;
}

const std::string &Platform::GetExternalStorageDirectory() { return external_storage_directory_; }

const std::string &Platform::GetTempDirectory() { return temp_directory_; }

Application &Platform::GetApp() {
    assert(active_app_ && "Application is not valid");
    return *active_app_;
}

Application &Platform::GetApp() const {
    assert(active_app_ && "Application is not valid");
    return *active_app_;
}

Window &Platform::GetWindow() { return *window_; }

std::vector<std::string> &Platform::GetArguments() { return Platform::arguments_; }

void Platform::SetArguments(const std::vector<std::string> &args) { arguments_ = args; }

void Platform::SetExternalStorageDirectory(const std::string &dir) { external_storage_directory_ = dir; }

void Platform::SetTempDirectory(const std::string &dir) { temp_directory_ = dir; }

std::vector<spdlog::sink_ptr> Platform::GetPlatformSinks() {
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    return sinks;
}

void Platform::SetApp(std::unique_ptr<Application> &&active_app) {
    if (active_app_) {
        auto execution_time = timer_.Stop();
        LOGI("Closing App (Runtime: {:.1f})", execution_time)

        auto app_id = active_app_->GetName();

        active_app_->Finish();
    }
    active_app_ = std::move(active_app);
}

bool Platform::StartApp() {
    active_app_->SetName("");

    if (!active_app_) {
        LOGE("Failed to create a valid vulkan app.")
        return false;
    }

    if (!active_app_->Prepare(*this)) {
        LOGE("Failed to prepare vulkan app.")
        return false;
    }

    OnAppStart("");

    return true;
}

void Platform::InputEvent(const vox::InputEvent &input_event) {
    if (process_input_events_ && active_app_) {
        active_app_->InputEvent(input_event);
    }

    if (input_event.GetSource() == EventSource::KEYBOARD) {
        const auto &key_event = static_cast<const KeyInputEvent &>(input_event);

        if (key_event.GetCode() == KeyCode::BACK || key_event.GetCode() == KeyCode::ESCAPE) {
            Close();
        }
    }
}

void Platform::Resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) {
    auto extent = Window::Extent{std::max<uint32_t>(win_width, min_window_width_),
                                 std::max<uint32_t>(win_height, min_window_height_)};
    if (window_) {
        window_->Resize(extent);
        if (active_app_) {
            active_app_->Resize(win_width, win_height, fb_width, fb_height);
        }
    }
}

#define HOOK(enum, func)                  \
    static auto res = hooks_.find(enum);  \
    if (res != hooks_.end()) {            \
        for (auto plugin : res->second) { \
            plugin->func;                 \
        }                                 \
    }

void Platform::OnPostDraw(RenderContext &context) { HOOK(Hook::POST_DRAW, OnPostDraw(context)) }

void Platform::OnAppError(const std::string &app_id) { HOOK(Hook::ON_APP_ERROR, OnAppError(app_id)) }

void Platform::OnUpdate(float delta_time) { HOOK(Hook::ON_UPDATE, OnUpdate(delta_time)) }

void Platform::OnAppStart(const std::string &app_id) { HOOK(Hook::ON_APP_START, OnAppStart(app_id)) }

void Platform::OnAppClose(const std::string &app_id) { HOOK(Hook::ON_APP_CLOSE, OnAppClose(app_id)) }

void Platform::OnPlatformClose() { HOOK(Hook::ON_PLATFORM_CLOSE, OnPlatformClose()) }

#undef HOOK

}  // namespace vox
