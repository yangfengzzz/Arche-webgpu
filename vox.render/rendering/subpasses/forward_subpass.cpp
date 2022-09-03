//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "forward_subpass.h"

#include "camera.h"
#include "components_manager.h"
#include "material/material.h"
#include "mesh/mesh.h"
#include "renderer.h"
#include "rendering/render_pass.h"
#include "shadow/shadow_manager.h"

namespace vox {
ForwardSubpass::ForwardSubpass(RenderContext *renderContext,
                               wgpu::TextureFormat depthStencilTextureFormat,
                               Scene *scene,
                               Camera *camera)
    : Subpass(renderContext, scene, camera), _depthStencilTextureFormat(depthStencilTextureFormat) {}

ForwardSubpass::RenderMode ForwardSubpass::renderMode() const { return _mode; }

void ForwardSubpass::setRenderMode(RenderMode mode) { _mode = mode; }

void ForwardSubpass::addRenderElement(const RenderElement &element) { _elements.emplace_back(element); }

void ForwardSubpass::clearAllRenderElement() { _elements.clear(); }

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
    passEncoder.PushDebugGroup("Draw Element");
    if (_mode == RenderMode::MANUAL) {
        auto compileMacros = ShaderMacroCollection();
        _scene->shaderData.mergeMacro(compileMacros, compileMacros);
        _camera->shaderData.mergeMacro(compileMacros, compileMacros);
        _drawElement(passEncoder, _elements, compileMacros);
    } else {
        _drawMeshes(passEncoder);
    }
    passEncoder.PopDebugGroup();
}

void ForwardSubpass::_drawMeshes(wgpu::RenderPassEncoder &passEncoder) {
    auto compileMacros = ShaderMacroCollection();
    _scene->shaderData.mergeMacro(compileMacros, compileMacros);
    _camera->shaderData.mergeMacro(compileMacros, compileMacros);

    std::vector<RenderElement> opaqueQueue;
    std::vector<RenderElement> alphaTestQueue;
    std::vector<RenderElement> transparentQueue;
    ComponentsManager::getSingleton().callRender(_camera, opaqueQueue, alphaTestQueue, transparentQueue);
    std::sort(opaqueQueue.begin(), opaqueQueue.end(), _compareFromNearToFar);
    std::sort(alphaTestQueue.begin(), alphaTestQueue.end(), _compareFromNearToFar);
    std::sort(transparentQueue.begin(), transparentQueue.end(), _compareFromFarToNear);

    _drawElement(passEncoder, opaqueQueue, compileMacros);
    _drawElement(passEncoder, alphaTestQueue, compileMacros);
    _drawElement(passEncoder, transparentQueue, compileMacros);
}

void ForwardSubpass::_drawElement(wgpu::RenderPassEncoder &passEncoder,
                                  const std::vector<RenderElement> &items,
                                  const ShaderMacroCollection &compileMacros) {
    for (auto &element : items) {
        auto macros = compileMacros;
        auto &renderer = element.renderer;
        uint32_t shadowCount = ShadowManager::shadowCount();
        if (renderer->receiveShadow && shadowCount != 0) {
            renderer->shaderData.enableMacro(SHADOW_MAP_COUNT, shadowCount);
        }
        uint32_t cubeShadowCount = ShadowManager::cubeShadowCount();
        if (renderer->receiveShadow && cubeShadowCount != 0) {
            renderer->shaderData.enableMacro(CUBE_SHADOW_MAP_COUNT, cubeShadowCount);
        }

        renderer->updateShaderData();
        renderer->shaderData.mergeMacro(macros, macros);
        auto &material = element.material;
        material->shaderData.mergeMacro(macros, macros);
        auto &mesh = element.mesh;
        auto &subMesh = element.subMesh;

        // PSO
        {
            const std::string &vertexSource = material->shader->vertexSource(macros);
            // std::cout<<vertexSource<<std::endl;
            const std::string &fragmentSource = material->shader->fragmentSource(macros);
            // std::cout<<fragmentSource<<std::endl;
            _forwardPipelineDescriptor.vertex.module = _pass->resourceCache().requestShader(vertexSource);
            _fragment.module = _pass->resourceCache().requestShader(fragmentSource);

            auto bindGroupLayoutDescriptors = material->shader->bindGroupLayoutDescriptors(macros);
            std::vector<wgpu::BindGroupLayout> bindGroupLayouts;
            for (auto &layoutDesc : bindGroupLayoutDescriptors) {
                wgpu::BindGroupLayout bindGroupLayout =
                        _pass->resourceCache().requestBindGroupLayout(layoutDesc.second);
                _bindGroupEntries.clear();
                _bindGroupEntries.resize(layoutDesc.second.entryCount);
                for (uint32_t i = 0; i < layoutDesc.second.entryCount; i++) {
                    auto &entry = layoutDesc.second.entries[i];
                    _bindGroupEntries[i].binding = entry.binding;
                    if (entry.buffer.type != wgpu::BufferBindingType::Undefined) {
                        _bindingData(_bindGroupEntries[i], material, renderer);
                    } else if (entry.texture.sampleType != wgpu::TextureSampleType::Undefined ||
                               entry.storageTexture.access != wgpu::StorageTextureAccess::Undefined) {
                        _bindingTexture(_bindGroupEntries[i], material, renderer);
                    } else if (entry.sampler.type != wgpu::SamplerBindingType::Undefined) {
                        _bindingSampler(_bindGroupEntries[i], material, renderer);
                    }
                }
                _bindGroupDescriptor.layout = bindGroupLayout;
                _bindGroupDescriptor.entryCount = layoutDesc.second.entryCount;
                _bindGroupDescriptor.entries = _bindGroupEntries.data();
                auto uniformBindGroup = _pass->resourceCache().requestBindGroup(_bindGroupDescriptor);
                passEncoder.SetBindGroup(layoutDesc.first, uniformBindGroup);
                bindGroupLayouts.emplace_back(std::move(bindGroupLayout));
            }
            material->shader->flush();

            _pipelineLayoutDescriptor.bindGroupLayoutCount = static_cast<uint32_t>(bindGroupLayouts.size());
            _pipelineLayoutDescriptor.bindGroupLayouts = bindGroupLayouts.data();
            _pipelineLayout = _pass->resourceCache().requestPipelineLayout(_pipelineLayoutDescriptor);
            _forwardPipelineDescriptor.layout = _pipelineLayout;

            material->renderState.apply(&_colorTargetState, &_depthStencil, _forwardPipelineDescriptor, passEncoder,
                                        true);

            _forwardPipelineDescriptor.vertex.bufferCount = static_cast<uint32_t>(mesh->vertexBufferLayouts().size());
            _forwardPipelineDescriptor.vertex.buffers = mesh->vertexBufferLayouts().data();
            _forwardPipelineDescriptor.primitive.topology = subMesh->topology();

            auto renderPipeline = _pass->resourceCache().requestPipeline(_forwardPipelineDescriptor);
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
}

void ForwardSubpass::_bindingData(wgpu::BindGroupEntry &entry, MaterialPtr mat, Renderer *renderer) {
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

void ForwardSubpass::_bindingTexture(wgpu::BindGroupEntry &entry, MaterialPtr mat, Renderer *renderer) {
    auto group = Shader::getShaderPropertyGroup(entry.binding);
    if (group.has_value()) {
        switch (*group) {
            case ShaderDataGroup::Scene:
                entry.textureView = _scene->shaderData.getTextureView(entry.binding).value();
                break;

            case ShaderDataGroup::Camera:
                entry.textureView = _camera->shaderData.getTextureView(entry.binding).value();
                break;

            case ShaderDataGroup::Renderer:
                entry.textureView = renderer->shaderData.getTextureView(entry.binding).value();
                break;

            case ShaderDataGroup::Material:
                entry.textureView = mat->shaderData.getTextureView(entry.binding).value();
                break;

            default:
                break;
        }
    }
}

void ForwardSubpass::_bindingSampler(wgpu::BindGroupEntry &entry, MaterialPtr mat, Renderer *renderer) {
    auto group = Shader::getShaderPropertyGroup(entry.binding);
    if (group.has_value()) {
        switch (*group) {
            case ShaderDataGroup::Scene:
                entry.sampler = _scene->shaderData.getSampler(entry.binding).value();
                break;

            case ShaderDataGroup::Camera:
                entry.sampler = _camera->shaderData.getSampler(entry.binding).value();
                break;

            case ShaderDataGroup::Renderer:
                entry.sampler = renderer->shaderData.getSampler(entry.binding).value();
                break;

            case ShaderDataGroup::Material:
                entry.sampler = mat->shaderData.getSampler(entry.binding).value();
                break;

            default:
                break;
        }
    }
}

}  // namespace vox
