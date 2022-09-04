//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/shadow/shadow_subpass.h"

#include "vox.render/camera.h"
#include "vox.render/components_manager.h"
#include "vox.render/material/material.h"
#include "vox.render/mesh/mesh.h"
#include "vox.render/renderer.h"
#include "vox.render/rendering/render_pass.h"
#include "vox.render/shadow/shadow_manager.h"

namespace vox {
ShadowSubpass::ShadowSubpass(RenderContext* renderContext, Scene* scene, Camera* camera)
    : Subpass(renderContext, scene, camera) {
    _material = std::make_shared<ShadowMaterial>(scene->device());
}

void ShadowSubpass::setShadowMaterial(const std::shared_ptr<ShadowMaterial>& mat) { _material = mat; }

void ShadowSubpass::setViewport(const std::optional<Vector4F>& viewport) { _viewport = viewport; }

void ShadowSubpass::prepare() {
    _depthStencil.format = ShadowManager::SHADOW_MAP_FORMAT;
    _shadowGenDescriptor.depthStencil = &_depthStencil;
    _shadowGenDescriptor.label = "Shadow Gen";
    { _shadowGenDescriptor.vertex.entryPoint = "main"; }
}

void ShadowSubpass::draw(wgpu::RenderPassEncoder& passEncoder) {
    passEncoder.PushDebugGroup("Shadow Map Pass");
    if (_viewport) {
        passEncoder.SetViewport(_viewport->x, _viewport->y, _viewport->z, _viewport->w, 0, 1);
    }

    _drawMeshes(passEncoder);
    passEncoder.PopDebugGroup();
}

void ShadowSubpass::_drawMeshes(wgpu::RenderPassEncoder& passEncoder) {
    auto compileMacros = ShaderVariant();
    _scene->shaderData.mergeVariants(compileMacros, compileMacros);
    _camera->shaderData.mergeVariants(compileMacros, compileMacros);

    std::vector<RenderElement> opaqueQueue;
    std::vector<RenderElement> alphaTestQueue;
    std::vector<RenderElement> transparentQueue;
    ComponentsManager::getSingleton().callRender(BoundingFrustum(_material->viewProjectionMatrix()), opaqueQueue,
                                                 alphaTestQueue, transparentQueue);
    std::sort(opaqueQueue.begin(), opaqueQueue.end(), _compareFromNearToFar);
    std::sort(alphaTestQueue.begin(), alphaTestQueue.end(), _compareFromNearToFar);
    std::sort(transparentQueue.begin(), transparentQueue.end(), _compareFromFarToNear);

    _drawElement(passEncoder, opaqueQueue, compileMacros);
    _drawElement(passEncoder, alphaTestQueue, compileMacros);
    _drawElement(passEncoder, transparentQueue, compileMacros);
}

void ShadowSubpass::_drawElement(wgpu::RenderPassEncoder& passEncoder,
                                 const std::vector<RenderElement>& items,
                                 const ShaderVariant& variant) {
    for (auto& element : items) {
        auto macros = variant;
        auto& renderer = element.renderer;
        if (renderer->castShadow) {
            renderer->updateShaderData();
            renderer->shaderData.mergeVariants(macros, macros);
            auto& mesh = element.mesh;
            auto& subMesh = element.subMesh;

            // PSO
            {
                auto& vert_shader_module = _pass->resourceCache().requestShaderModule(
                        wgpu::ShaderStage::Vertex, *_material->vertex_source_, macros);
                _shadowGenDescriptor.vertex.module = vert_shader_module.handle();

                _bindGroupLayoutEntryVecMap.clear();
                _bindGroupEntryVecMap.clear();
                _scene->shaderData.bindData(vert_shader_module.GetResources(), _bindGroupLayoutEntryVecMap,
                                            _bindGroupEntryVecMap);
                _camera->shaderData.bindData(vert_shader_module.GetResources(), _bindGroupLayoutEntryVecMap,
                                             _bindGroupEntryVecMap);
                renderer->shaderData.bindData(vert_shader_module.GetResources(), _bindGroupLayoutEntryVecMap,
                                              _bindGroupEntryVecMap);
                _material->shaderData.bindData(vert_shader_module.GetResources(), _bindGroupLayoutEntryVecMap,
                                               _bindGroupEntryVecMap);

                std::vector<wgpu::BindGroupLayout> bindGroupLayouts;
                for (const auto& bindGroupLayoutEntryVec : _bindGroupLayoutEntryVecMap) {
                    bindGroupLayoutDescriptor.entries = bindGroupLayoutEntryVec.second.data();
                    bindGroupLayoutDescriptor.entryCount = bindGroupLayoutEntryVec.second.size();
                    wgpu::BindGroupLayout bindGroupLayout =
                            _pass->resourceCache().requestBindGroupLayout(bindGroupLayoutDescriptor);

                    const auto group = bindGroupLayoutEntryVec.first;
                    const auto& bindGroupEntryVec = _bindGroupEntryVecMap[group];
                    _bindGroupDescriptor.layout = bindGroupLayout;
                    _bindGroupDescriptor.entryCount = bindGroupEntryVec.size();
                    _bindGroupDescriptor.entries = bindGroupEntryVec.data();
                    auto uniformBindGroup = _pass->resourceCache().requestBindGroup(_bindGroupDescriptor);
                    passEncoder.SetBindGroup(group, uniformBindGroup);
                    bindGroupLayouts.emplace_back(std::move(bindGroupLayout));
                }

                _pipelineLayoutDescriptor.bindGroupLayoutCount = static_cast<uint32_t>(bindGroupLayouts.size());
                _pipelineLayoutDescriptor.bindGroupLayouts = bindGroupLayouts.data();
                _pipelineLayout = _pass->resourceCache().requestPipelineLayout(_pipelineLayoutDescriptor);
                _shadowGenDescriptor.layout = _pipelineLayout;

                _material->renderState.apply(nullptr, &_depthStencil, _shadowGenDescriptor, passEncoder, true);

                _shadowGenDescriptor.vertex.bufferCount = static_cast<uint32_t>(mesh->vertexBufferLayouts().size());
                _shadowGenDescriptor.vertex.buffers = mesh->vertexBufferLayouts().data();
                _shadowGenDescriptor.primitive.topology = subMesh->topology();

                auto renderPipeline = _pass->resourceCache().requestPipeline(_shadowGenDescriptor);
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
            }
            passEncoder.DrawIndexed(subMesh->count(), 1, subMesh->start(), 0, 0);
        }
    }
}

}  // namespace vox
