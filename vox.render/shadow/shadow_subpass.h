//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/rendering/subpass.h"
#include "vox.render/shadow/shadow_material.h"

namespace vox {
class ShadowSubpass : public Subpass {
public:
    ShadowSubpass(RenderContext* renderContext, Scene* scene, Camera* camera);

    void setShadowMaterial(const std::shared_ptr<ShadowMaterial>& mat);

    void setViewport(const std::optional<Vector4F>& viewport);

    void prepare() override;

    void draw(wgpu::RenderPassEncoder& passEncoder) override;

private:
    void _drawMeshes(wgpu::RenderPassEncoder& passEncoder);

    void _drawElement(wgpu::RenderPassEncoder& passEncoder,
                      const std::vector<RenderElement>& items,
                      const ShaderVariant& variant);

    wgpu::RenderPipelineDescriptor _shadowGenDescriptor;
    wgpu::DepthStencilState _depthStencil;

    std::unordered_map<uint32_t, std::vector<wgpu::BindGroupLayoutEntry>> _bindGroupLayoutEntryVecMap;
    std::unordered_map<uint32_t, std::vector<wgpu::BindGroupEntry>> _bindGroupEntryVecMap;
    wgpu::BindGroupDescriptor _bindGroupDescriptor;
    wgpu::BindGroupLayoutDescriptor bindGroupLayoutDescriptor;
    std::vector<wgpu::BindGroupEntry> _bindGroupEntries{};

    wgpu::PipelineLayoutDescriptor _pipelineLayoutDescriptor;
    wgpu::PipelineLayout _pipelineLayout;

    std::optional<Vector4F> _viewport = std::nullopt;
    std::shared_ptr<ShadowMaterial> _material{nullptr};
};

}  // namespace vox