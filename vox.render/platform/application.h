//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>

#include "vox.base/timer.h"
#include "vox.render/debug_info.h"
#include "vox.render/platform/configuration.h"
#include "vox.render/platform/input_events.h"

namespace vox {
class Window;

class Platform;

class Application {
public:
    Application();

    virtual ~Application() = default;

    /**
     * @brief Prepares the application for execution
     * @param platform The platform the application is being run on
     */
    virtual bool Prepare(Platform &platform);

    /**
     * @brief Updates the application
     * @param delta_time The time since the last update
     */
    virtual void Update(float delta_time);

    /**
     * @brief Handles cleaning up the application
     */
    virtual void Finish();

    /**
     * @brief Handles resizing of the window
     * @param win_width New width of the window
     * @param win_height New height of the window
     * @param fb_width New width of the framebuffer
     * @param fb_height New height of the framebuffer
     */
    virtual bool Resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height);

    /**
     * @brief Handles input events of the window
     * @param input_event The input event object
     */
    virtual void InputEvent(const InputEvent &input_event);

    [[nodiscard]] const std::string &GetName() const;

    void SetName(const std::string &name);

    DebugInfo &GetDebugInfo();

protected:
    float fps_{0.0f};

    float frame_time_{0.0f};  // In ms

    uint32_t frame_count_{0};

    uint32_t last_frame_count_{0};

    Platform *platform_{nullptr};

private:
    std::string name_{};

    // The debug info of the app
    DebugInfo debug_info_{};
};

}  // namespace vox
