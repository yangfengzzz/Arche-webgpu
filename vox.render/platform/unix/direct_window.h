//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <termios.h>
#include <unistd.h>

#include "vox.render/platform/platform.h"
#include "vox.render/platform/window.h"
#include "vox.render/vk_common.h"

namespace vox {
/**
 * @brief Direct2Display window
 */
class DirectWindow : public Window {
public:
    DirectWindow(Platform *platform, const Window::Properties &properties);

    virtual ~DirectWindow();

    virtual VkSurfaceKHR CreateSurface(Instance &instance) override;

    virtual VkSurfaceKHR CreateSurface(VkInstance instance, VkPhysicalDevice physical_device) override;

    virtual bool ShouldClose() override;

    virtual void ProcessEvents() override;

    virtual void Close() override;

    float GetDpiFactor() const override;

private:
    void PollTerminal();

    uint32_t FindCompatiblePlane(VkPhysicalDevice phys_dev,
                                 VkDisplayKHR display,
                                 const std::vector<VkDisplayPlanePropertiesKHR> &plane_properties);

private:
    mutable bool keep_running = true;
    Platform *platform = nullptr;
    float dpi;
    int tty_fd;
    struct termios termio;
    struct termios termio_prev;
    KeyCode key_down = KeyCode::UNKNOWN;
};

}  // namespace vox
