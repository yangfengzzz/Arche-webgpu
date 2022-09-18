//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/camera.h"
#include "vox.render/material/material.h"
#include "vox.render/mesh/mesh.h"
#include "vox.render/renderer.h"
#include "vox.render/rendering/resource_cache.h"
#include "vox.render/rendering/subpasses/geometry_subpass.h"

namespace vox {
ForwardSubpass::ForwardSubpass(RenderContext *renderContext,
                               wgpu::TextureFormat depthStencilTextureFormat,
                               Scene *scene,
                               Camera *camera)
    : Subpass(renderContext, scene, camera), _depthStencilTextureFormat(depthStencilTextureFormat) {}

void ForwardSubpass::prepare() {
    _depthStencil.format = _depthStencilTextureFormat;
    _forwardPipelineDescriptor.depthStencil = &_depthStencil;
    _colorTargetState.format = _renderContext->drawableTextureFormat();
    _fragment.targetCount = 1;
    _fragment.targets = &_colorTargetState;
    _forwardPipelineDescriptor.fragment = &_fragment;
    _forwardPipelineDescriptor.label = "Forward Pipeline";
    {
        _forwardPipelineDescriptor.vertex.entryPoint = "main";
        _fragment.entryPoint = "main";
    }
}

void ForwardSubpass::draw(wgpu::RenderPassEncoder &passEncoder) {
    auto compile_variant = ShaderVariant();
    _scene->shaderData.mergeVariants(compile_variant, compile_variant);
    if (_camera) {
        _camera->shaderData.mergeVariants(compile_variant, compile_variant);
    }

    passEncoder.PushDebugGroup("Draw Element");
    _drawElement(passEncoder, compile_variant);
    passEncoder.PopDebugGroup();
}

void ForwardSubpass::_drawElement(wgpu::RenderPassEncoder &passEncoder,
                                  const RenderElement &element,
                                  const ShaderVariant &variant) {
    auto macros = variant;
    auto &renderer = element.renderer;
    renderer->updateShaderData();
    renderer->shaderData.mergeVariants(macros, macros);

    auto &material = element.material;
    material->shaderData.mergeVariants(macros, macros);

    auto &mesh = element.mesh;
    auto &subMesh = element.subMesh;

    // PSO
    {
        _bindGroupLayoutEntryVecMap.clear();
        _bindGroupEntryVecMap.clear();
        auto &vert_shader_module = ResourceCache::GetSingleton().requestShaderModule(wgpu::ShaderStage::Vertex,
                                                                                     *material->vertex_source_, macros);
        _forwardPipelineDescriptor.vertex.module = vert_shader_module.handle();
        _scene->shaderData.bindData(vert_shader_module.GetResources(), _bindGroupLayoutEntryVecMap,
                                    _bindGroupEntryVecMap);
        _camera->shaderData.bindData(vert_shader_module.GetResources(), _bindGroupLayoutEntryVecMap,
                                     _bindGroupEntryVecMap);
        renderer->shaderData.bindData(vert_shader_module.GetResources(), _bindGroupLayoutEntryVecMap,
                                      _bindGroupEntryVecMap);
        material->shaderData.bindData(vert_shader_module.GetResources(), _bindGroupLayoutEntryVecMap,
                                      _bindGroupEntryVecMap);

        if (fragmentEnabled) {
            auto &frag_shader_module = ResourceCache::GetSingleton().requestShaderModule(
                    wgpu::ShaderStage::Fragment, *material->fragment_source_, macros);
            _fragment.module = frag_shader_module.handle();
            _scene->shaderData.bindData(frag_shader_module.GetResources(), _bindGroupLayoutEntryVecMap,
                                        _bindGroupEntryVecMap);
            _camera->shaderData.bindData(frag_shader_module.GetResources(), _bindGroupLayoutEntryVecMap,
                                         _bindGroupEntryVecMap);
            renderer->shaderData.bindData(frag_shader_module.GetResources(), _bindGroupLayoutEntryVecMap,
                                          _bindGroupEntryVecMap);
            material->shaderData.bindData(frag_shader_module.GetResources(), _bindGroupLayoutEntryVecMap,
                                          _bindGroupEntryVecMap);
        }

        std::vector<wgpu::BindGroupLayout> bindGroupLayouts;
        for (const auto &bindGroupLayoutEntryVec : _bindGroupLayoutEntryVecMap) {
            bindGroupLayoutDescriptor.entries = bindGroupLayoutEntryVec.second.data();
            bindGroupLayoutDescriptor.entryCount = bindGroupLayoutEntryVec.second.size();
            wgpu::BindGroupLayout bindGroupLayout =
                    ResourceCache::GetSingleton().requestBindGroupLayout(bindGroupLayoutDescriptor);

            const auto group = bindGroupLayoutEntryVec.first;
            const auto &bindGroupEntryVec = _bindGroupEntryVecMap[group];
            _bindGroupDescriptor.layout = bindGroupLayout;
            _bindGroupDescriptor.entryCount = bindGroupEntryVec.size();
            _bindGroupDescriptor.entries = bindGroupEntryVec.data();
            auto uniformBindGroup = ResourceCache::GetSingleton().requestBindGroup(_bindGroupDescriptor);
            passEncoder.SetBindGroup(group, uniformBindGroup);
            bindGroupLayouts.emplace_back(std::move(bindGroupLayout));
        }

        _pipelineLayoutDescriptor.bindGroupLayoutCount = static_cast<uint32_t>(bindGroupLayouts.size());
        _pipelineLayoutDescriptor.bindGroupLayouts = bindGroupLayouts.data();
        _pipelineLayout = ResourceCache::GetSingleton().requestPipelineLayout(_pipelineLayoutDescriptor);
        _forwardPipelineDescriptor.layout = _pipelineLayout;

        material->renderState.apply(&_colorTargetState, &_depthStencil, _forwardPipelineDescriptor, passEncoder, true);

        _forwardPipelineDescriptor.vertex.bufferCount = static_cast<uint32_t>(mesh->vertexBufferLayouts().size());
        _forwardPipelineDescriptor.vertex.buffers = mesh->vertexBufferLayouts().data();
        _forwardPipelineDescriptor.primitive.topology = subMesh->topology();

        auto renderPipeline = ResourceCache::GetSingleton().requestPipeline(_forwardPipelineDescriptor);
        passEncoder.SetPipeline(renderPipeline);
    }

    // Draw Call
    for (uint32_t j = 0; j < mesh->vertexBufferBindings().size(); j++) {
        auto vertexBufferBinding = mesh->vertexBufferBindings()[j];
        if (vertexBufferBinding) {
            passEncoder.SetVertexBuffer(j, mesh->vertexBufferBindings()[j]->handle());
        }
    }
    auto indexBufferBinding = mesh->indexBufferBinding();
    if (indexBufferBinding) {
        passEncoder.SetIndexBuffer(mesh->indexBufferBinding()->buffer(), mesh->indexBufferBinding()->format());
        passEncoder.DrawIndexed(subMesh->count(), mesh->instanceCount(), subMesh->start(), 0, 0);
    } else {
        passEncoder.Draw(subMesh->count(), mesh->instanceCount());
    }
}

}  // namespace vox