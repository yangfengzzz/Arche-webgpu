//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/rendering/subpass.h"

namespace vox {
class ForwardSubpass : public Subpass {
public:
    enum class RenderMode { AUTO, MANUAL };

    ForwardSubpass(RenderContext* renderContext,
                   wgpu::TextureFormat depthStencilTextureFormat,
                   Scene* scene,
                   Camera* camera);

    void prepare() override;

    void draw(wgpu::RenderPassEncoder& passEncoder) override;

public:
    [[nodiscard]] RenderMode renderMode() const;

    void setRenderMode(RenderMode mode);

    void addRenderElement(const RenderElement& element);

    void clearAllRenderElement();

private:
    void _drawElement(wgpu::RenderPassEncoder& passEncoder,
                      const std::vector<RenderElement>& items,
                      const ShaderVariant& variant);

    wgpu::RenderPipelineDescriptor _forwardPipelineDescriptor;
    wgpu::DepthStencilState _depthStencil;
    wgpu::FragmentState _fragment;
    wgpu::ColorTargetState _colorTargetState;

    std::unordered_map<uint32_t, std::vector<wgpu::BindGroupLayoutEntry>> _bindGroupLayoutEntryVecMap;
    std::unordered_map<uint32_t, std::vector<wgpu::BindGroupEntry>> _bindGroupEntryVecMap;
    wgpu::BindGroupDescriptor _bindGroupDescriptor;
    wgpu::BindGroupLayoutDescriptor bindGroupLayoutDescriptor;

    wgpu::PipelineLayoutDescriptor _pipelineLayoutDescriptor;
    wgpu::PipelineLayout _pipelineLayout;

    wgpu::TextureFormat _depthStencilTextureFormat;

    RenderMode _mode = RenderMode::AUTO;
    std::vector<RenderElement> _elements{};
};

}  // namespace vox