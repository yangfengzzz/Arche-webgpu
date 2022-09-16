//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/rendering/subpasses/skybox_subpass.h"

#include <utility>

#include "vox.render/camera.h"
#include "vox.render/mesh/primitive_mesh.h"
#include "vox.render/rendering/render_pass.h"
#include "vox.render/rendering/resource_cache.h"

namespace vox {
SkyboxSubpass::SkyboxSubpass(RenderContext* renderContext,
                             wgpu::TextureFormat depthStencilTextureFormat,
                             Scene* scene,
                             Camera* camera)
    : Subpass(renderContext, scene, camera),
      _depthStencilTextureFormat(depthStencilTextureFormat),
      _vpMatrix(renderContext->device(), sizeof(Matrix4x4F), wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst),
      vert_shader_("skybox.vert"),
      frag_shader_("skybox.frag") {
    createCuboid();
}

void SkyboxSubpass::createSphere(float radius) {
    _mesh = PrimitiveMesh::createSphere(_renderContext->device(), radius);
    _type = SkyBoxType::Sphere;
}

void SkyboxSubpass::createCuboid() {
    _mesh = PrimitiveMesh::createCuboid(_renderContext->device(), 1, 1, 1);
    _type = SkyBoxType::Cuboid;
}

std::shared_ptr<Image> SkyboxSubpass::textureCubeMap() { return _cubeMap; }

void SkyboxSubpass::setTextureCubeMap(const std::shared_ptr<Image>& v) { _cubeMap = v; }

void SkyboxSubpass::setTextureCubeSampler(const wgpu::SamplerDescriptor& desc) { _samplerDesc = desc; }

// MARK: - Render
void SkyboxSubpass::prepare() {
    _depthStencil.format = _depthStencilTextureFormat;
    _depthStencil.depthWriteEnabled = false;
    _depthStencil.depthCompare = wgpu::CompareFunction::LessEqual;
    _forwardPipelineDescriptor.depthStencil = &_depthStencil;
    _colorTargetState.format = _renderContext->drawableTextureFormat();
    _fragment.targetCount = 1;
    _fragment.targets = &_colorTargetState;
    _forwardPipelineDescriptor.fragment = &_fragment;
    _forwardPipelineDescriptor.label = "Skybox Pipeline";
    // Shader
    {
        ShaderVariant variant;
        _forwardPipelineDescriptor.vertex.entryPoint = "main";
        _forwardPipelineDescriptor.vertex.module =
                ResourceCache::GetSingleton()
                        .requestShaderModule(wgpu::ShaderStage::Vertex, vert_shader_, variant)
                        .handle();
        _fragment.entryPoint = "main";
        _fragment.module = ResourceCache::GetSingleton()
                                   .requestShaderModule(wgpu::ShaderStage::Fragment, frag_shader_, variant)
                                   .handle();
    }
    // BindGroupLayout
    {
        _bindGroupLayoutEntry.resize(3);
        _bindGroupLayoutEntry[0].binding = 10;
        _bindGroupLayoutEntry[0].visibility = wgpu::ShaderStage::Vertex;
        _bindGroupLayoutEntry[0].buffer.type = wgpu::BufferBindingType::Uniform;
        _bindGroupLayoutEntry[1].binding = 0;
        _bindGroupLayoutEntry[1].visibility = wgpu::ShaderStage::Fragment;
        _bindGroupLayoutEntry[1].texture.multisampled = false;
        _bindGroupLayoutEntry[1].texture.sampleType = wgpu::TextureSampleType::Float;
        _bindGroupLayoutEntry[1].texture.viewDimension = wgpu::TextureViewDimension::Cube;
        _bindGroupLayoutEntry[2].binding = 1;
        _bindGroupLayoutEntry[2].visibility = wgpu::ShaderStage::Fragment;
        _bindGroupLayoutEntry[2].sampler.type = wgpu::SamplerBindingType::Filtering;
        _bindGroupLayoutDescriptor.entryCount = static_cast<uint32_t>(_bindGroupLayoutEntry.size());
        _bindGroupLayoutDescriptor.entries = _bindGroupLayoutEntry.data();
        _bindGroupLayout = ResourceCache::GetSingleton().requestBindGroupLayout(_bindGroupLayoutDescriptor);
    }
    // BindGroup
    {
        _bindGroupEntries.resize(3);
        _bindGroupEntries[0].binding = 10;
        _bindGroupEntries[0].size = sizeof(Matrix4x4F);
        _bindGroupEntries[0].buffer = _vpMatrix.handle();
        _bindGroupEntries[1].binding = 0;
        _bindGroupEntries[2].binding = 1;
        _bindGroupDescriptor.entryCount = static_cast<uint32_t>(_bindGroupEntries.size());
        _bindGroupDescriptor.entries = _bindGroupEntries.data();
        _bindGroupDescriptor.layout = _bindGroupLayout;
    }
    // PipelineLayout
    {
        _pipelineLayoutDescriptor.bindGroupLayoutCount = 1;
        _pipelineLayoutDescriptor.bindGroupLayouts = &_bindGroupLayout;
        _pipelineLayout = ResourceCache::GetSingleton().requestPipelineLayout(_pipelineLayoutDescriptor);
        _forwardPipelineDescriptor.layout = _pipelineLayout;
    }
    // RenderPipeline
    {
        _forwardPipelineDescriptor.primitive.frontFace = wgpu::FrontFace::CW;
        _forwardPipelineDescriptor.primitive.cullMode = wgpu::CullMode::Back;
        _forwardPipelineDescriptor.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
        _forwardPipelineDescriptor.vertex.bufferCount = static_cast<uint32_t>(_mesh->vertexBufferLayouts().size());
        _forwardPipelineDescriptor.vertex.buffers = _mesh->vertexBufferLayouts().data();
        _renderPipeline = ResourceCache::GetSingleton().requestPipeline(_forwardPipelineDescriptor);
    }
}

void SkyboxSubpass::draw(wgpu::RenderPassEncoder& passEncoder) {
    passEncoder.PushDebugGroup("Draw Skybox");

    const auto projectionMatrix = _camera->projectionMatrix();
    auto viewMatrix = _camera->viewMatrix();
    if (_type == SkyBoxType::Cuboid) {
        viewMatrix[12] = 0;
        viewMatrix[13] = 0;
        viewMatrix[14] = 0;
        viewMatrix[15] = 1;
    }
    auto _matrix = projectionMatrix * viewMatrix;
    std::vector<uint8_t> bytes = to_bytes(_matrix);
    _renderContext->device().GetQueue().WriteBuffer(_vpMatrix.handle(), 0, bytes.data(), sizeof(Matrix4x4F));

    _bindGroupEntries[1].textureView = _cubeMap->getImageView(wgpu::TextureViewDimension::Cube)->handle();
    _bindGroupEntries[2].sampler = ResourceCache::GetSingleton().requestSampler(_samplerDesc);
    passEncoder.SetBindGroup(0, ResourceCache::GetSingleton().requestBindGroup(_bindGroupDescriptor));
    passEncoder.SetPipeline(_renderPipeline);

    // Draw Call
    for (uint32_t j = 0; j < _mesh->vertexBufferBindings().size(); j++) {
        auto vertexBufferBinding = _mesh->vertexBufferBindings()[j];
        if (vertexBufferBinding) {
            passEncoder.SetVertexBuffer(j, _mesh->vertexBufferBindings()[j]->handle());
        }
    }
    auto indexBufferBinding = _mesh->indexBufferBinding();
    if (indexBufferBinding) {
        passEncoder.SetIndexBuffer(_mesh->indexBufferBinding()->buffer(), _mesh->indexBufferBinding()->format());
    }
    passEncoder.DrawIndexed(_mesh->subMesh()->count(), 1, _mesh->subMesh()->start(), 0, 0);

    passEncoder.PopDebugGroup();
}

}  // namespace vox
