//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/quaternion.h"
#include "vox.math/vector3.h"
#include "vox.render/material/base_material.h"
#include "vox.render/rendering/render_pass.h"
#include "vox.render/ui/widgets/panel_transformables/panel_window.h"
#include "vox.render/ui/widgets/visual/image.h"

namespace vox {
using namespace ui;

namespace editor::ui {
/**
 * Base class for any view
 */
class View : public PanelWindow {
public:
    /**
     * Constructor
     * @param p_title p_title
     * @param p_opened p_opened
     * @param p_windowSettings p_windowSettings
     */
    View(const std::string& p_title,
         bool p_opened,
         const PanelWindowSettings& p_windowSettings,
         RenderContext* renderContext);

    /**
     * Update the view
     * @param p_deltaTime p_deltaTime
     */
    virtual void update(float p_deltaTime);

    /**
     * Custom implementation of the draw method
     */
    void DrawImpl() override;

    /**
     * Render the view
     */
    virtual void render(wgpu::CommandEncoder& commandEncoder) = 0;

    /**
     * Returns the size of the panel ignoring its titlebar height
     */
    [[nodiscard]] std::pair<uint16_t, uint16_t> safeSize() const;

public:
    ModelMeshPtr createPlane(wgpu::Device& device);

    /**
     * Returns the grid color of the view
     */
    [[nodiscard]] const Vector3F& gridColor() const;

    /**
     * Defines the grid color of the view
     * @param p_color p_color
     */
    void setGridColor(const Vector3F& p_color);

protected:
    RenderContext* _renderContext{nullptr};
    Vector3F _gridColor = {0.176f, 0.176f, 0.176f};

    std::unique_ptr<RenderPass> _renderPass{nullptr};

    vox::ui::ImageUI* _image{nullptr};
    wgpu::Texture _texture;
    wgpu::TextureDescriptor _textureDesc;
    wgpu::TextureView _depthStencilTexture;
    wgpu::TextureFormat _depthStencilTextureFormat = wgpu::TextureFormat::Depth24PlusStencil8;
    bool _createRenderTexture(uint32_t width, uint32_t height);

    wgpu::RenderPassDescriptor _renderPassDescriptor;
    wgpu::RenderPassColorAttachment _renderPassColorAttachments;
    wgpu::RenderPassDepthStencilAttachment _renderPassDepthStencilAttachment;
};

// MARK: - Grid
class GridMaterial : public BaseMaterial {
public:
    explicit GridMaterial(wgpu::Device& device) : BaseMaterial(device) { setIsTransparent(true); }
};

}  // namespace editor::ui
}  // namespace vox