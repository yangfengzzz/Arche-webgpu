//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/platform/headless_window.h"

namespace vox {
HeadlessWindow::HeadlessWindow(const Window::Properties &properties) : Window(properties) {}

VkSurfaceKHR HeadlessWindow::CreateSurface(Instance &instance) { return VK_NULL_HANDLE; }

VkSurfaceKHR HeadlessWindow::CreateSurface(VkInstance, VkPhysicalDevice) { return VK_NULL_HANDLE; }

bool HeadlessWindow::ShouldClose() { return closed_; }

void HeadlessWindow::Close() { closed_ = true; }

float HeadlessWindow::GetDpiFactor() const {
    // This factor is used for scaling UI elements, so return 1.0f (1 x n = n)
    return 1.0f;
}

}  // namespace vox
