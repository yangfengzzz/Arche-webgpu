//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/components_manager.h"
#include "vox.render/graphics_application.h"
#include "vox.render/image_manager.h"
#include "vox.render/lighting/light_manager.h"
#include "vox.render/mesh/mesh_manager.h"
#include "vox.render/particle/particle_manager.h"
#include "vox.render/physics/physics_manager.h"
#include "vox.render/rendering/resource_cache.h"
#include "vox.render/scene_manager.h"
#include "vox.render/shader/shader_manager.h"
#include "vox.render/shadow/shadow_manager.h"

namespace vox {
class ForwardApplication : public GraphicsApplication {
public:
    ForwardApplication() = default;

    ~ForwardApplication() override;

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

    virtual void loadScene() = 0;

    virtual void updateGPUTask(wgpu::CommandEncoder& commandEncoder);

    Camera* mainCamera();

protected:
    Camera* _mainCamera{nullptr};

    wgpu::RenderPassDescriptor _guiPassDescriptor;
    wgpu::RenderPassDescriptor _renderPassDescriptor;
    wgpu::RenderPassColorAttachment _colorAttachments;

    wgpu::RenderPassColorAttachment _guiColorAttachments;
    wgpu::RenderPassDepthStencilAttachment _depthStencilAttachment;

    /**
     * @brief Pipeline used for rendering, it should be set up by the concrete sample
     */
    std::unique_ptr<RenderPass> _renderPass{nullptr};

    /**
     * @brief Holds all scene information
     */
    std::unique_ptr<ShaderManager> shader_manager_{nullptr};
    std::unique_ptr<MeshManager> mesh_manager_{nullptr};
    std::unique_ptr<ImageManager> image_manager_{nullptr};
    std::unique_ptr<ResourceCache> resource_cache_{nullptr};

    std::unique_ptr<ComponentsManager> _componentsManager{nullptr};
    std::unique_ptr<SceneManager> _sceneManager{nullptr};
    std::unique_ptr<ShadowManager> _shadowManager{nullptr};
    std::unique_ptr<LightManager> _lightManager{nullptr};
    std::unique_ptr<ParticleManager> _particleManager{nullptr};
    std::unique_ptr<PhysicsManager> _physicsManager{nullptr};

protected:
    wgpu::TextureView _depthStencilTexture;
    wgpu::TextureFormat _depthStencilTextureFormat = wgpu::TextureFormat::Depth24PlusStencil8;
    wgpu::TextureView _createDepthStencilView(uint32_t width, uint32_t height);
};

}  // namespace vox