//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.editor/editor_actions.h"
#include "vox.editor/editor_resources.h"
#include "vox.editor/panels_manager.h"
#include "vox.render/components_manager.h"
#include "vox.render/graphics_application.h"
#include "vox.render/image_manager.h"
#include "vox.render/lighting/light_manager.h"
#include "vox.render/mesh/mesh_manager.h"
#include "vox.render/particle/particle_manager.h"
#include "vox.render/physics/physics_manager.h"
#include "vox.render/scene_manager.h"
#include "vox.render/shader/shader_manager.h"
#include "vox.render/shadow/shadow_manager.h"

namespace vox::editor {
class EditorApplication : public GraphicsApplication {
public:
    explicit EditorApplication(std::string editorAssetsPath = "./assets/Editor/");

    ~EditorApplication() override;

    /**
     * @brief Additional sample initialization
     */
    bool prepare(Platform& platform) override;

    /**
     * @brief Main loop sample events
     */
    void update(float delta_time) override;

    bool resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) override;

    void inputEvent(const InputEvent& inputEvent) override;

    virtual void updateGPUTask(wgpu::CommandEncoder& commandEncoder);

    /**
     * Handle panels creation and canvas binding
     */
    virtual void setupUI() = 0;

    /**
     * Render every views (Scene View, Game View, Asset View)
     */
    virtual void updateViews(float deltaTime, wgpu::CommandEncoder& commandEncoder) {}

protected:
    friend class EditorActions;
    const std::string editorAssetsPath;

    ::vox::ui::Canvas _canvas;
    ui::PanelsManager _panelsManager;
    std::unique_ptr<EditorResources> _editorResources{nullptr};
    std::unique_ptr<EditorActions> _editorActions{nullptr};

    float delta_time_{};
    Camera* _mainCamera{nullptr};

    wgpu::RenderPassDescriptor _renderPassDescriptor;
    wgpu::RenderPassColorAttachment _colorAttachments;

    /**
     * @brief Holds all scene information
     */
    std::unique_ptr<ImageManager> image_manager_{nullptr};
    std::unique_ptr<ShaderManager> shader_manager_{nullptr};
    std::unique_ptr<MeshManager> mesh_manager_{nullptr};
    std::unique_ptr<ResourceCache> resource_cache_{nullptr};

    std::unique_ptr<ComponentsManager> _componentsManager{nullptr};
    std::unique_ptr<SceneManager> _sceneManager{nullptr};
    std::unique_ptr<ShadowManager> _shadowManager{nullptr};
    std::unique_ptr<LightManager> _lightManager{nullptr};
    //    std::unique_ptr<ParticleManager> _particleManager{nullptr};
    std::unique_ptr<PhysicsManager> _physicsManager{nullptr};
};

}  // namespace vox::editor
