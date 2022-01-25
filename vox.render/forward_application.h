//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef forward_hpp
#define forward_hpp

#include "graphics_application.h"
//#include "lighting/shadow_manager.h"

namespace vox {
class ForwardApplication: public GraphicsApplication {
public:
    ForwardApplication() = default;
    
    virtual ~ForwardApplication();
    
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
    
    virtual void loadScene(uint32_t width, uint32_t height) = 0;
    
protected:
    Camera* _mainCamera{nullptr};
    
    /**
     * @brief Holds all scene information
     */
    std::unique_ptr<Scene> _scene{nullptr};
    
    wgpu::RenderPassDescriptor _renderPassDescriptor;
    
    std::unique_ptr<wgpu::RenderPassColorAttachment> _colorAttachments{nullptr};
    std::unique_ptr<wgpu::RenderPassDepthStencilAttachment> _depthStencilAttachment{nullptr};
    
    /**
     * @brief Pipeline used for rendering, it should be set up by the concrete sample
     */
    std::unique_ptr<RenderPass> _renderPass{nullptr};
    
    //    std::unique_ptr<ShadowManager> _shadowManager{nullptr};
};

}

#endif /* forward_hpp */
