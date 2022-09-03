//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/platform/window.h"
#include "vox.render/vk_common.h"

struct GLFWwindow;

namespace vox {
class Platform;

/**
 * @brief An implementation of GLFW, inheriting the behaviour of the Window interface
 */
class GlfwWindow : public Window {
public:
    GlfwWindow(Platform *platform, const Window::Properties &properties);

    ~GlfwWindow() override;

    VkSurfaceKHR CreateSurface(Instance &instance) override;

    VkSurfaceKHR CreateSurface(VkInstance instance, VkPhysicalDevice physical_device) override;

    bool ShouldClose() override;

    void ProcessEvents() override;

    void Close() override;

    [[nodiscard]] float GetDpiFactor() const override;

    [[nodiscard]] float GetContentScaleFactor() const override;

    GLFWwindow *Handle();

private:
    GLFWwindow *handle_ = nullptr;
};

}  // namespace vox
