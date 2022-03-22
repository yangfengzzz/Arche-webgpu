//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef scene_view_hpp
#define scene_view_hpp

#include "view.h"
#include "controls/orbit_control.h"
#include "imgui_zmo.h"

namespace vox {
using namespace ui;
class ColorPickerSubpass;

namespace editor {
namespace ui {
class SceneView : public View {
public:
    /**
     * Constructor
     * @param p_title p_title
     * @param p_opened p_opened
     * @param p_windowSettings p_windowSettings
     */
    SceneView(const std::string &p_title, bool p_opened,
              const PanelWindowSettings &p_windowSettings,
              RenderContext* renderContext, Scene* scene);
    
    /**
     * Custom implementation of the draw method
     */
    void _draw_Impl() override;
    
    /**
     * Update the scene view
     */
    void update(float p_deltaTime) override;
    
    /**
     * Custom implementation of the render method
     */
    void render(wgpu::CommandEncoder& commandEncoder) override;
    
    /**
     * Pick the object at the screen coordinate position.
     * @param offsetX Relative X coordinate of the canvas
     * @param offsetY Relative Y coordinate of the canvas
     */
    void pick(float offsetX, float offsetY);
    
    void inputEvent(const InputEvent &inputEvent);
    
    void loadScene(EntityPtr& rootEntity);
    
private:
    float _camDistance = 8.f;
    ImGuizmo::OPERATION _currentGizmoOperation = ImGuizmo::TRANSLATE;
    void editTransform(float *cameraView, float *cameraProjection, float *matrix);

private:
    Camera* _mainCamera{nullptr};
    Scene* _scene{nullptr};
    control::OrbitControl* _cameraControl{nullptr};
    
private:
    bool _needPick;
    Vector2F _pickPos;
    
    wgpu::TextureDescriptor _colorPickerTextureDesc;
    wgpu::Texture _colorPickerTexture;
    wgpu::RenderPassDescriptor _colorPickerPassDescriptor;
    wgpu::RenderPassColorAttachment _colorPickerColorAttachments;
    wgpu::RenderPassDepthStencilAttachment _colorPickerDepthStencilAttachment;
    
    std::unique_ptr<RenderPass> _colorPickerRenderPass{nullptr};
    ColorPickerSubpass* _colorPickerSubpass{nullptr};
    std::pair<Renderer *, MeshPtr> _pickResult;
    
    std::array<uint8_t, 4> _pixel{};
    wgpu::Buffer _stageBuffer;
    void _copyRenderTargetToBuffer(wgpu::CommandEncoder& commandEncoder);
    void _readColorFromRenderTarget();
    
    wgpu::ImageCopyTexture _imageCopyTexture;
    wgpu::ImageCopyBuffer _imageCopyBuffer;
    wgpu::Extent3D _extent;
};


}
}
}
#endif /* scene_view_hpp */
