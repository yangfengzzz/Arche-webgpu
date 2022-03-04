//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef shadow_subpass_hpp
#define shadow_subpass_hpp

#include "rendering/render_subpass.h"
#include "shadow_material.h"

namespace vox {
class ShadowSubpass: public RenderSubpass {
public:
    ShadowSubpass(RenderContext* renderContext,
                  Scene* scene,
                  Camera* camera);
    
    void setShadowMaterial(const std::shared_ptr<ShadowMaterial>& mat);
    
    void setViewport(const std::optional<Vector4F>& viewport);
    
    void prepare() override;
    
    void draw(wgpu::RenderPassEncoder& passEncoder) override;
    
private:
    void _drawMeshes(wgpu::RenderPassEncoder &passEncoder);
    
    void _drawElement(wgpu::RenderPassEncoder &passEncoder,
                      const std::vector<RenderElement> &items,
                      const ShaderMacroCollection& compileMacros);
    
    void _bindingData(wgpu::BindGroupEntry& entry,
                      MaterialPtr mat, Renderer* renderer);
    
    wgpu::RenderPipelineDescriptor _shadowGenDescriptor;
    wgpu::DepthStencilState _depthStencil;
    
    wgpu::BindGroupDescriptor _bindGroupDescriptor;
    std::vector<wgpu::BindGroupEntry> _bindGroupEntries{};
    
    wgpu::PipelineLayoutDescriptor _pipelineLayoutDescriptor;
    wgpu::PipelineLayout _pipelineLayout;
    
    std::optional<Vector4F> _viewport = std::nullopt;
    std::shared_ptr<ShadowMaterial> _material{nullptr};
};

}


#endif /* shadow_subpass_hpp */
