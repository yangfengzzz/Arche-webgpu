//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <variant>

#include "vox.editor/view/view.h"
#include "vox.render/rendering/subpasses/geometry_subpass.h"
#include "vox.toolkit/controls/orbit_control.h"

namespace vox {
using namespace ui;

namespace editor::ui {
/**
 * Provide a view for assets
 */
class AssetView : public View {
public:
    /**
     * Constructor
     * @param p_title p_title
     * @param p_opened p_opened
     * @param p_windowSettings p_windowSettings
     */
    AssetView(const std::string& p_title,
              bool p_opened,
              const PanelWindowSettings& p_windowSettings,
              RenderContext* renderContext,
              Scene* scene);

    /**
     * Update the scene view
     */
    void update(float p_deltaTime) override;

    /**
     * Custom implementation of the render method
     */
    void render(wgpu::CommandEncoder& commandEncoder) override;

    void loadScene(Entity* rootEntity);

private:
    Camera* _mainCamera{nullptr};
    Scene* _scene{nullptr};
    ForwardSubpass* _subpass{nullptr};
    std::vector<RenderElement> _elements{};

    control::OrbitControl* _cameraControl{nullptr};
};

}  // namespace editor::ui
}  // namespace vox
