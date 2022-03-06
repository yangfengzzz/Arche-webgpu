//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shadow_subpass.h"
#include "material/material.h"
#include "graphics/mesh.h"
#include "camera.h"
#include "renderer.h"
#include "rendering/render_pass.h"
#include "shadow_manager.h"

namespace vox {
ShadowSubpass::ShadowSubpass(RenderContext* renderContext,
                             Scene* scene,
                             Camera* camera):
Subpass(renderContext, scene, camera) {
    _material = std::make_shared<ShadowMaterial>(scene->device());
}

void ShadowSubpass::setShadowMaterial(const std::shared_ptr<ShadowMaterial>& mat) {
    _material = mat;
}

void ShadowSubpass::setViewport(const std::optional<Vector4F>& viewport) {
    _viewport = viewport;
}

void ShadowSubpass::prepare() {
    _depthStencil.format = ShadowManager::SHADOW_MAP_FORMAT;
    _shadowGenDescriptor.depthStencil = &_depthStencil;
    _shadowGenDescriptor.label = "Shadow Gen";
    {
        _shadowGenDescriptor.vertex.entryPoint = "main";
    }
}

void ShadowSubpass::draw(wgpu::RenderPassEncoder& passEncoder) {
    passEncoder.PushDebugGroup("Shadow Map Pass");
    if (_viewport) {
        passEncoder.SetViewport(_viewport->x, _viewport->y, _viewport->z, _viewport->w, 0, 1);
    }
    
    _drawMeshes(passEncoder);
    passEncoder.PopDebugGroup();
}

void ShadowSubpass::_drawMeshes(wgpu::RenderPassEncoder &passEncoder) {
    auto compileMacros = ShaderMacroCollection();
    _scene->shaderData.mergeMacro(compileMacros, compileMacros);
    _camera->shaderData.mergeMacro(compileMacros, compileMacros);
    
    std::vector<RenderElement> opaqueQueue;
    std::vector<RenderElement> alphaTestQueue;
    std::vector<RenderElement> transparentQueue;
    _scene->_componentsManager.callRender(BoundingFrustum(_material->viewProjectionMatrix()),
                                          opaqueQueue, alphaTestQueue, transparentQueue);
    std::sort(opaqueQueue.begin(), opaqueQueue.end(), _compareFromNearToFar);
    std::sort(alphaTestQueue.begin(), alphaTestQueue.end(), _compareFromNearToFar);
    std::sort(transparentQueue.begin(), transparentQueue.end(), _compareFromFarToNear);
    
    _drawElement(passEncoder, opaqueQueue, compileMacros);
    _drawElement(passEncoder, alphaTestQueue, compileMacros);
    _drawElement(passEncoder, transparentQueue, compileMacros);
}

void ShadowSubpass::_drawElement(wgpu::RenderPassEncoder &passEncoder,
                                 const std::vector<RenderElement> &items,
                                 const ShaderMacroCollection& compileMacros) {
    for (auto &element : items) {
        auto macros = compileMacros;
        auto& renderer = element.renderer;
        if (renderer->castShadow) {
            renderer->updateShaderData(_camera->viewMatrix(), _camera->projectionMatrix());
            renderer->shaderData.mergeMacro(macros, macros);
            auto& mesh = element.mesh;
            auto& subMesh = element.subMesh;
            
            // PSO
            {
                const std::string& vertexSource = _material->shader->vertexSource(macros);
                // std::cout<<vertexSource<<std::endl;
                _shadowGenDescriptor.vertex.module = _pass->resourceCache().requestShader(vertexSource);
                
                auto bindGroupLayoutDescriptors = _material->shader->bindGroupLayoutDescriptors(macros);
                std::vector<wgpu::BindGroupLayout> bindGroupLayouts;
                for (auto& layoutDesc : bindGroupLayoutDescriptors) {
                    wgpu::BindGroupLayout bindGroupLayout = _pass->resourceCache().requestBindGroupLayout(layoutDesc.second);
                    _bindGroupEntries.clear();
                    _bindGroupEntries.resize(layoutDesc.second.entryCount);
                    for (uint32_t i = 0; i < layoutDesc.second.entryCount; i++) {
                        auto& entry = layoutDesc.second.entries[i];
                        _bindGroupEntries[i].binding = entry.binding;
                        if (entry.buffer.type != wgpu::BufferBindingType::Undefined) {
                            _bindingData(_bindGroupEntries[i], _material, renderer);
                        }
                    }
                    _bindGroupDescriptor.layout = bindGroupLayout;
                    _bindGroupDescriptor.entryCount = layoutDesc.second.entryCount;
                    _bindGroupDescriptor.entries = _bindGroupEntries.data();
                    auto uniformBindGroup = _pass->resourceCache().requestBindGroup(_bindGroupDescriptor);
                    passEncoder.SetBindGroup(layoutDesc.first, uniformBindGroup);
                    bindGroupLayouts.emplace_back(std::move(bindGroupLayout));
                }
                _material->shader->flush();
                
                _pipelineLayoutDescriptor.bindGroupLayoutCount = static_cast<uint32_t>(bindGroupLayouts.size());
                _pipelineLayoutDescriptor.bindGroupLayouts = bindGroupLayouts.data();
                _pipelineLayout = _pass->resourceCache().requestPipelineLayout(_pipelineLayoutDescriptor);
                _shadowGenDescriptor.layout = _pipelineLayout;
                
                _material->renderState.apply(nullptr, &_depthStencil,
                                             _shadowGenDescriptor, passEncoder, true);
                
                _shadowGenDescriptor.vertex.bufferCount = static_cast<uint32_t>(mesh->vertexBufferLayouts().size());
                _shadowGenDescriptor.vertex.buffers = mesh->vertexBufferLayouts().data();
                _shadowGenDescriptor.primitive.topology = subMesh->topology();
                
                auto renderPipeline = _pass->resourceCache().requestPipeline(_shadowGenDescriptor);
                passEncoder.SetPipeline(renderPipeline);
            }
            
            // Draw Call
            for (uint32_t j = 0; j < mesh->vertexBufferBindings().size(); j++) {
                auto vertexBufferBinding =  mesh->vertexBufferBindings()[j];
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

void ShadowSubpass::_bindingData(wgpu::BindGroupEntry& entry,
                                 MaterialPtr mat, Renderer* renderer) {
    auto group = Shader::getShaderPropertyGroup(entry.binding);
    if (group.has_value()) {
        switch (*group) {
            case ShaderDataGroup::Scene:
                entry.buffer = _scene->shaderData.getData(entry.binding)->handle();
                entry.size = _scene->shaderData.getData(entry.binding)->size();
                break;
                
            case ShaderDataGroup::Camera:
                entry.buffer = _camera->shaderData.getData(entry.binding)->handle();
                entry.size = _camera->shaderData.getData(entry.binding)->size();
                break;
                
            case ShaderDataGroup::Renderer:
                entry.buffer = renderer->shaderData.getData(entry.binding)->handle();
                entry.size = renderer->shaderData.getData(entry.binding)->size();
                break;
                
            case ShaderDataGroup::Material:
                entry.buffer = mat->shaderData.getData(entry.binding)->handle();
                entry.size = mat->shaderData.getData(entry.binding)->size();
                break;
                
            default:
                break;
        }
    }
}

}
