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

    std::unique_ptr<BackendBinding> createBackendBinding(wgpu::Device &device) override { return nullptr; }

    bool ShouldClose() override;

    void Close() override;

    [[nodiscard]] float GetDpiFactor() const override;

private:
    bool closed_{false};
};

}  // namespace vox
