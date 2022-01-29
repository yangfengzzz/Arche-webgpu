//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.
#ifndef forward_subpass_hpp
#define forward_subpass_hpp

#include "../subpass.h"

namespace vox {
class ForwardSubpass: public Subpass {
public:
    ForwardSubpass(RenderContext* renderContext,
                   Scene* scene,
                   Camera* camera);
    
    void prepare() override;
    
    void draw(wgpu::RenderPassEncoder& passEncoder) override;
    
private:
    void _drawMeshes(wgpu::RenderPassEncoder &passEncoder);
    
    void _drawElement(wgpu::RenderPassEncoder &passEncoder,
                      const std::vector<RenderElement> &items,
                      const ShaderMacroCollection& compileMacros);
    
    void _bindingData(wgpu::BindGroupEntry& entry,
                      MaterialPtr mat, Renderer* renderer);
    
    wgpu::RenderPipelineDescriptor _forwardPipelineDescriptor;
    wgpu::DepthStencilState _depthStencil;
    wgpu::FragmentState _fragment;
    wgpu::ColorTargetState _colorTargetState;

    wgpu::BindGroupDescriptor _bindGroupDescriptor;
    std::vector<wgpu::BindGroupEntry> _bindGroupEntries{};

    wgpu::PipelineLayoutDescriptor _pipelineLayoutDescriptor;
    wgpu::PipelineLayout _pipelineLayout;
};

}

#endif /* forward_subpass_hpp */
