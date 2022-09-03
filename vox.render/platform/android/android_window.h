//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <android_native_app_glue.h>

#include "vox.render/platform/window.h"
#include "vox.render/vk_common.h"

namespace vox {
class AndroidPlatform;

/**
 * @brief Wrapper for a ANativeWindow, handles the window behaviour (including headless mode on Android)
 *        This class should not be responsible for destroying the underlying data it points to
 */
class AndroidWindow : public Window {
public:
    /**
     * @brief Constructor
     * @param platform The platform this window is created for
     * @param window A reference to the location of the Android native window
     * @param properties Window configuration
     */
    AndroidWindow(AndroidPlatform *platform, ANativeWindow *&window, const Window::Properties &properties);

    virtual ~AndroidWindow() = default;

    /**
     * @brief Creates a Vulkan surface to the native window
     *        If headless, this will return VK_NULL_HANDLE
     */
    virtual VkSurfaceKHR create_surface(Instance &instance) override;

    /**
     * @brief Creates a Vulkan surface to the native window
     *        If headless, this will return nullptr
     */
    virtual VkSurfaceKHR create_surface(VkInstance instance, VkPhysicalDevice physical_device) override;

    virtual void process_events() override;

    virtual bool should_close() override;

    virtual void close() override;

    virtual float get_dpi_factor() const override;

private:
    AndroidPlatform *platform;

    // Handle to the android window
    ANativeWindow *&handle;

    bool finish_called{false};
};

}  // namespace vox
