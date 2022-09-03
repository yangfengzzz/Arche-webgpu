//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/platform/android/android_window.h"

#include "vox.render/platform/android/android_platform.h"

namespace vox {
AndroidWindow::AndroidWindow(AndroidPlatform *platform, ANativeWindow *&window, const Window::Properties &properties)
    : Window(properties), handle{window}, platform{platform} {}

VkSurfaceKHR AndroidWindow::create_surface(Instance &instance) {
    return create_surface(instance.GetHandle(), VK_NULL_HANDLE);
}

VkSurfaceKHR AndroidWindow::create_surface(VkInstance instance, VkPhysicalDevice) {
    if (instance == VK_NULL_HANDLE || !handle || properties.mode == Mode::Headless) {
        return VK_NULL_HANDLE;
    }

    VkSurfaceKHR surface{};

    VkAndroidSurfaceCreateInfoKHR info{VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR};

    info.window = handle;

    VK_CHECK(vkCreateAndroidSurfaceKHR(instance, &info, nullptr, &surface));

    return surface;
}

void AndroidWindow::process_events() { process_android_events(platform->get_android_app()); }

bool AndroidWindow::should_close() { return finish_called ? true : handle == nullptr; }

void AndroidWindow::close() {
    ANativeActivity_finish(platform->get_activity());
    finish_called = true;
}

float AndroidWindow::get_dpi_factor() const {
    return AConfiguration_getDensity(platform->get_android_app()->config) /
           static_cast<float>(ACONFIGURATION_DENSITY_MEDIUM);
}

}  // namespace vox
