//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/platform/window.h"

namespace vox {
/**
 * @brief Surface-less implementation of a Window, for use in headless rendering
 */
class HeadlessWindow : public Window {
public:
    explicit HeadlessWindow(const Window::Properties &properties);

    ~HeadlessWindow() override = default;

    /**
     * @brief A direct window doesn't have a surface
     * @returns VK_NULL_HANDLE
     */
    VkSurfaceKHR CreateSurface(Instance &instance) override;

    /**
     * @brief A direct window doesn't have a surface
     * @returns nullptr
     */
    VkSurfaceKHR CreateSurface(VkInstance instance, VkPhysicalDevice physical_device) override;

    bool ShouldClose() override;

    void Close() override;

    [[nodiscard]] float GetDpiFactor() const override;

private:
    bool closed_{false};
};

}  // namespace vox
