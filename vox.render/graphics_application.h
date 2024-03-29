//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/platform/application.h"
#include "vox.render/rendering/render_pass.h"
#include "vox.render/scene.h"
#include "vox.render/ui/ui_manager.h"

namespace vox {
class GraphicsApplication : public Application {
public:
    GraphicsApplication();

    ~GraphicsApplication() override;

    /**
     * @brief Additional sample initialization
     */
    bool prepare(Platform &platform) override;

    /**
     * @brief Main loop sample events
     */
    void update(float delta_time) override;

    bool resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) override;

    void inputEvent(const InputEvent &inputEvent) override;

    void finish() override;

private:
    void _createCppDawnDevice();

protected:
    std::unique_ptr<dawn_native::Instance> _instance{nullptr};

    /**
     * @brief The WebGPU device
     */
    wgpu::Device _device;

    /**
     * @brief View used for rendering, it is responsible for managing the frames and their underlying images
     */
    std::unique_ptr<RenderContext> _renderContext{nullptr};

    std::unique_ptr<ui::UiManager> _gui{nullptr};
};

}  // namespace vox