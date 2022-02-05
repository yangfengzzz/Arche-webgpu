//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "skybox_subpass.h"
#include "rendering/render_pass.h"
#include "mesh/primitive_mesh.h"
#include "camera.h"

namespace vox {
SkyboxSubpass::SkyboxSubpass(RenderContext* renderContext,
                             Scene* scene,
                             Camera* camera):
Subpass(renderContext, scene, camera),
_vpMatrix(renderContext->device(), sizeof(Matrix4x4F),
          wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst){
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

SampledTextureCubePtr SkyboxSubpass::textureCubeMap() {
    return _cubeMap;
}

void SkyboxSubpass::setTextureCubeMap(SampledTextureCubePtr v) {
    _cubeMap = v;
}

//MARK: - Render
void SkyboxSubpass::prepare() {
    _colorTargetState.format = _renderContext->drawableTextureFormat();
    _fragment.targetCount = 1;
    _fragment.targets = &_colorTargetState;
    _forwardPipelineDescriptor.fragment = &_fragment;
    _forwardPipelineDescriptor.label = "Skybox Pipeline";
    // Shader
    {
        ShaderProgram* program = _pass->resourceCache().requestShader(_vertexSource, _fragmentSource);
        _forwardPipelineDescriptor.vertex.entryPoint = "main";
        _forwardPipelineDescriptor.vertex.module = program->vertexShader();
        _fragment.entryPoint = "main";
        _fragment.module = program->fragmentShader();
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
        _bindGroupLayout = _pass->resourceCache().requestBindGroupLayout(_bindGroupLayoutDescriptor);
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
        _bindGroup = _pass->resourceCache().requestBindGroup(_bindGroupDescriptor);
    }
    // PipelineLayout
    {
        _pipelineLayoutDescriptor.bindGroupLayoutCount = 1;
        _pipelineLayoutDescriptor.bindGroupLayouts = &_bindGroupLayout;
        _pipelineLayout = _pass->resourceCache().requestPipelineLayout(_pipelineLayoutDescriptor);
        _forwardPipelineDescriptor.layout = _pipelineLayout;
    }
    // RenderPipeline
    {
        _forwardPipelineDescriptor.vertex.bufferCount = static_cast<uint32_t>(_mesh->vertexBufferLayouts().size());
        _forwardPipelineDescriptor.vertex.buffers = _mesh->vertexBufferLayouts().data();
        _renderPipeline = _pass->resourceCache().requestRenderPipeline(_forwardPipelineDescriptor);
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
    
    _bindGroupEntries[1].textureView = _cubeMap->textureView();
    _bindGroupEntries[2].sampler = _cubeMap->sampler();
    
    passEncoder.SetPipeline(_renderPipeline);
    
    // Draw Call
    for (uint32_t j = 0; j < _mesh->vertexBufferBindings().size(); j++) {
        auto vertexBufferBinding =  _mesh->vertexBufferBindings()[j];
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

}
