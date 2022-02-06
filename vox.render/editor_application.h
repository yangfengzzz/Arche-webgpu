//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef editor_application_hpp
#define editor_application_hpp

#include "forward_application.h"

namespace vox {
class ColorPickerSubpass;

class EditorApplication: public ForwardApplication {
public:
    EditorApplication() = default;
    
    virtual ~EditorApplication();
    
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
    
public:
    virtual void pickFunctor(Renderer *renderer, MeshPtr mesh) {};
    
    /**
     * Pick the object at the screen coordinate position.
     * @param offsetX Relative X coordinate of the canvas
     * @param offsetY Relative Y coordinate of the canvas
     */
    void pick(float offsetX, float offsetY);
    
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
    
    std::array<uint8_t, 4> _pixel{};
    wgpu::Buffer _stageBuffer;
    void _copyRenderTargetToBuffer(wgpu::CommandEncoder& commandEncoder);
    void _readColorFromRenderTarget();
    
    wgpu::ImageCopyTexture _imageCopyTexture;
    wgpu::ImageCopyBuffer _imageCopyBuffer;
    wgpu::Extent3D _extent;
};

}

#endif /* editor_application_hpp */
