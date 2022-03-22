//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "color_picker_subpass.h"
#include "rendering/render_pass.h"
#include "material/material.h"
#include "graphics/mesh.h"
#include "renderer.h"
#include "camera.h"

namespace vox {
ColorPickerSubpass::ColorPickerSubpass(RenderContext* renderContext,
                                       wgpu::TextureFormat depthStencilTextureFormat,
                                       Scene* scene,
                                       Camera* camera):
Subpass(renderContext, scene, camera),
_depthStencilTextureFormat(depthStencilTextureFormat) {
    _material = std::make_shared<UnlitMaterial>(renderContext->device());
}

void ColorPickerSubpass::prepare() {
    _depthStencil.format = _depthStencilTextureFormat;
    _forwardPipelineDescriptor.depthStencil = &_depthStencil;
    _colorTargetState.format = wgpu::TextureFormat::BGRA8Unorm;
    _fragment.targetCount = 1;
    _fragment.targets = &_colorTargetState;
    _forwardPipelineDescriptor.fragment = &_fragment;
    _forwardPipelineDescriptor.label = "ColorPicker Pipeline";
    {
        _forwardPipelineDescriptor.vertex.entryPoint = "main";
        _fragment.entryPoint = "main";
    }
}

void ColorPickerSubpass::draw(wgpu::RenderPassEncoder& passEncoder) {
    _currentId = 0;
    _primitivesMap.clear();
    
    passEncoder.PushDebugGroup("Draw ColorPicker");
    _drawMeshes(passEncoder);
    passEncoder.PopDebugGroup();
}

void ColorPickerSubpass::_drawMeshes(wgpu::RenderPassEncoder &passEncoder) {
    auto compileMacros = ShaderMacroCollection();
    _scene->shaderData.mergeMacro(compileMacros, compileMacros);
    _camera->shaderData.mergeMacro(compileMacros, compileMacros);
    
    std::vector<RenderElement> opaqueQueue;
    std::vector<RenderElement> alphaTestQueue;
    std::vector<RenderElement> transparentQueue;
    _scene->_componentsManager.callRender(_camera, opaqueQueue, alphaTestQueue, transparentQueue);
    std::sort(opaqueQueue.begin(), opaqueQueue.end(), _compareFromNearToFar);
    std::sort(alphaTestQueue.begin(), alphaTestQueue.end(), _compareFromNearToFar);
    std::sort(transparentQueue.begin(), transparentQueue.end(), _compareFromFarToNear);
    
    // prealloc buffer
    size_t total = opaqueQueue.size() + alphaTestQueue.size() + transparentQueue.size();
    _bufferPool.reserve(total);
    for (size_t i = _bufferPool.size(); i < total; i++) {
        _bufferPool.push_back(Buffer(_renderContext->device(), sizeof(Color), wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst));
    }
    
    _drawElement(passEncoder, opaqueQueue, compileMacros);
    _drawElement(passEncoder, alphaTestQueue, compileMacros);
    _drawElement(passEncoder, transparentQueue, compileMacros);
}

void ColorPickerSubpass::_drawElement(wgpu::RenderPassEncoder &passEncoder,
                                      const std::vector<RenderElement> &items,
                                      const ShaderMacroCollection& compileMacros) {
    for (auto &element : items) {
        auto macros = compileMacros;
        auto& renderer = element.renderer;
        renderer->updateShaderData();
        renderer->shaderData.mergeMacro(macros, macros);
        auto& mesh = element.mesh;
        auto& subMesh = element.subMesh;
        
        _primitivesMap[_currentId] = std::make_pair(renderer, mesh);
        Vector3F color = id2Color(_currentId);
        auto reverseColor = Color(color.z, color.y, color.x, 1);
        Buffer& buffer = _bufferPool[_currentId];
        buffer.uploadData(_renderContext->device(), &reverseColor, sizeof(Color));
        _currentId += 1;

        // PSO
        {
            const std::string& vertexSource = _material->shader->vertexSource(macros);
            // std::cout<<vertexSource<<std::endl;
            const std::string& fragmentSource = _material->shader->fragmentSource(macros);
            // std::cout<<fragmentSource<<std::endl;
            _forwardPipelineDescriptor.vertex.module = _pass->resourceCache().requestShader(vertexSource);
            _fragment.module = _pass->resourceCache().requestShader(fragmentSource);
            
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
                        _bindingData(_bindGroupEntries[i], buffer, renderer);
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
            _forwardPipelineDescriptor.layout = _pipelineLayout;
            
            _material->renderState.apply(&_colorTargetState, &_depthStencil,
                                         _forwardPipelineDescriptor, passEncoder, true);
            
            _forwardPipelineDescriptor.vertex.bufferCount = static_cast<uint32_t>(mesh->vertexBufferLayouts().size());
            _forwardPipelineDescriptor.vertex.buffers = mesh->vertexBufferLayouts().data();
            _forwardPipelineDescriptor.primitive.topology = subMesh->topology();
            
            auto renderPipeline = _pass->resourceCache().requestPipeline(_forwardPipelineDescriptor);
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

void ColorPickerSubpass::_bindingData(wgpu::BindGroupEntry& entry,
                                      Buffer& buffer, Renderer* renderer) {
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
                entry.buffer = buffer.handle();
                entry.size = sizeof(Color);
                break;
                
            default:
                break;
        }
    }
}

//MARK: - Picker
Vector3F ColorPickerSubpass::id2Color(uint32_t id) {
    if (id >= 0xffffff) {
        std::cout << "Framebuffer Picker encounter primitive's id greater than " + std::to_string(0xffffff)
        << std::endl;
        return Vector3F(0, 0, 0);
    }
    
    return Vector3F((id & 0xff) / 255.0, ((id & 0xff00) >> 8) / 255.0, ((id & 0xff0000) >> 16) / 255.0);
}

uint32_t ColorPickerSubpass::color2Id(const std::array<uint8_t, 4> &color) {
    return color[0] | (color[1] << 8) | (color[2] << 16);
}

std::pair<Renderer *, MeshPtr> ColorPickerSubpass::getObjectByColor(const std::array<uint8_t, 4> &color) {
    auto iter = _primitivesMap.find(color2Id(color));
    if (iter != _primitivesMap.end()) {
        return iter->second;
    } else {
        return std::make_pair(nullptr, nullptr);
    }
}

}
