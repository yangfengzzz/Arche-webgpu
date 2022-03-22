//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef editor_application_hpp
#define editor_application_hpp

#include "graphics_application.h"
#include "shadow/shadow_manager.h"
#include "lighting/light_manager.h"
#include "particle/particle_manager.h"
#include "scene_manager.h"

#include "panels_manager.h"

namespace vox {
namespace editor {
class EditorApplication: public GraphicsApplication {
public:
    EditorApplication();
    
    /**
     * @brief Additional sample initialization
     */
    bool prepare(Engine &engine) override;
    
    /**
     * @brief Main loop sample events
     */
    void update(float delta_time) override;
    
    bool resize(uint32_t win_width, uint32_t win_height,
                uint32_t fb_width, uint32_t fb_height) override;
    
    void inputEvent(const InputEvent &inputEvent) override;
    
    virtual void updateGPUTask(wgpu::CommandEncoder& commandEncoder);
    
    /**
     * Handle panels creation and canvas binding
     */
    void setupUI();
    
    /**
     * Render every views (Scene View, Game View, Asset View)
     */
    void renderViews(float deltaTime, wgpu::CommandEncoder& commandEncoder);
    
    /**
     * Update editor panels
     * @param deltaTime deltaTime
     */
    void updateEditorPanels(float deltaTime);
    
protected:
    ::vox::ui::Canvas _canvas;
    ui::PanelsManager _panelsManager;
    
    Camera* _mainCamera{nullptr};
    
    wgpu::RenderPassDescriptor _renderPassDescriptor;
    wgpu::RenderPassColorAttachment _colorAttachments;
    
    /**
     * @brief Holds all scene information
     */
    std::unique_ptr<SceneManager> _sceneManager{nullptr};
    std::unique_ptr<ShadowManager> _shadowManager{nullptr};
    std::unique_ptr<LightManager> _lightManager{nullptr};
    std::unique_ptr<ParticleManager> _particleManager{nullptr};
};

}
}
#endif /* editor_application_hpp */
