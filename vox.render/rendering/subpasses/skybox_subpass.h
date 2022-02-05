//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef skybox_subpass_hpp
#define skybox_subpass_hpp

#include "../subpass.h"
#include "texture/sampled_texturecube.h"

namespace vox {
class SkyboxSubpass: public Subpass {
public:
    SkyboxSubpass(RenderContext* renderContext,
                  Scene* scene,
                  Camera* camera);
    
    void prepare() override;
    
    void draw(wgpu::RenderPassEncoder& passEncoder) override;
    
public:
    void createSphere(float radius);
    
    void createCuboid();
    
    /**
     * Texture cube map of the sky box material.
     */
    SampledTextureCubePtr textureCubeMap();
    
    void setTextureCubeMap(SampledTextureCubePtr v);
    
private:
    enum class SkyBoxType {
        Cuboid, Sphere
    };
    SkyBoxType _type;
    ModelMeshPtr _mesh{nullptr};
    SampledTextureCubePtr _cubeMap{nullptr};
    Buffer _vpMatrix;
    
    WGSLPtr _vertexSource{nullptr};
    WGSLPtr _fragmentSource{nullptr};
    
    wgpu::RenderPipelineDescriptor _forwardPipelineDescriptor;
    wgpu::FragmentState _fragment;
    wgpu::ColorTargetState _colorTargetState;
    
    std::vector<wgpu::BindGroupLayoutEntry> _bindGroupLayoutEntry;
    wgpu::BindGroupLayoutDescriptor _bindGroupLayoutDescriptor;
    wgpu::BindGroupLayout _bindGroupLayout;
    
    std::vector<wgpu::BindGroupEntry> _bindGroupEntries{};
    wgpu::BindGroupDescriptor _bindGroupDescriptor;
    wgpu::BindGroup _bindGroup;

    wgpu::PipelineLayoutDescriptor _pipelineLayoutDescriptor;
    wgpu::PipelineLayout _pipelineLayout;
    
    wgpu::RenderPipeline _renderPipeline;
};

}

#endif /* skybox_subpass_hpp */
